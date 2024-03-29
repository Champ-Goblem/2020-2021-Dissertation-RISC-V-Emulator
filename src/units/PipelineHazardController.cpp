#include "../include/units/PipelineHazardController.h"
#include "../include/instructions/AbstractInstruction.h"
#include "../include/hw/RegisterFile.h"
#include "../include/instructions/RType.h"

PipelineHazardController::PipelineHazardController(ushort XLEN, RegisterFile* registerFile, bool isRV32E) {
  this->XLEN = XLEN;
  this->registerFile = registerFile;
  this->isRV32E = isRV32E;
  this->executingQueue = vector<InstructionQueueEntry>(NUM_STAGES);
}

// Moves the pipeline along one position and adds empty
// item for decode stage
void PipelineHazardController::bump() {
  lock_guard<mutex> lck(lock);
  InstructionQueueEntry instructionQueueEntry {};

  this->executingQueue.push_back(instructionQueueEntry);
  this->executingQueue.erase(this->executingQueue.begin());
  if (usedQueueSize < NUM_STAGES) {
    usedQueueSize += 1;
  }
}

// Update code stage with new instruction
void PipelineHazardController::enqueue(AbstractInstruction instruction) {
  lock_guard<mutex> lck(lock);
  this->executingQueue[STAGE_DECODE].instruction = instruction;
}

// This function checks if there is a dependency on the execute stage
// It is designed to only be called from decode stage, assumes any stages
// after the decode stage with data dependencies have posted their result
// already so there should be no issues
bool PipelineHazardController::checkForStaleRegister(ushort reg) {
  lock_guard<mutex> lck(lock);
  if (reg >= (this->isRV32E ? 16 : 32)) {
    throw PipelineHazardException("Failed to check hazards, register invalid");
  }

  if (reg == 0) {
    return false;
  }

  /* if (usedQueueSize >= (NUM_STAGES - STAGE_DECODE) && executingQueue[STAGE_DECODE].instruction.getRD() == reg) {
    return true;
  } else  */if (usedQueueSize >= (NUM_STAGES - STAGE_EXECUTE) && executingQueue[STAGE_EXECUTE].instruction.getRD() == reg) {
    return true;
  } else if (usedQueueSize >= (NUM_STAGES - STAGE_MEM) && executingQueue[STAGE_MEM].instruction.getRD() == reg && executingQueue[STAGE_MEM].RDVal.size() == 0) {
    return true;
  }

  return false;
}

bytes PipelineHazardController::fetchRegisterValue(ushort reg) {
  // Performs some rudimentary checks to ensure consistency
  // Fetches the result stored in the structure executingQueue
  // in order to pass it back to the decode stage
  lock_guard<mutex> lck(lock);
  if (reg >= (this->isRV32E ? 16 : 32)) {
    throw PipelineHazardException("Failed to fetch data for register, register invalid");
  }

  if (reg == 0) {
    return bytes(this->XLEN);
  }

  if (usedQueueSize >= (NUM_STAGES - STAGE_EXECUTE) && executingQueue[STAGE_EXECUTE].instruction.getRD() == reg) {
    // TODO: Should we throw here?
    throw PipelineHazardException("Failed to fetch data for register, has a dependency on the execute stage, no value calculated yet");
  } else if (usedQueueSize >= (NUM_STAGES - STAGE_MEM) && executingQueue[STAGE_MEM].instruction.getRD() == reg) {
    if (executingQueue[STAGE_MEM].RDVal.size() == 0) {
      throw PipelineHazardException("Failed to fetch data for register, RD result is zero bytes long");
    }

    return executingQueue[STAGE_MEM].RDVal;
  } else if (usedQueueSize >= (NUM_STAGES - STAGE_WB) && executingQueue[STAGE_WB].instruction.getRD() == reg) {
    if (executingQueue[STAGE_WB].RDVal.size() == 0) {
      throw PipelineHazardException("Failed to fetch data for register, RD result is zero bytes long");
    }

    return executingQueue[STAGE_WB].RDVal;
  }

  return this->registerFile->get(reg);
}

void PipelineHazardController::storeResultAfterExecution(bytes result) {
  // Updates the result entry of a corresponding instruction entry in the pipeline
  // used after an instruction executes in order to pass the value back
  lock_guard<mutex> lck(lock);
  if (result.size() == 0 || result.size() != XLEN) {
    throw PipelineHazardException("Failed to store result for RD after execution, result is wrong size");
  }

  if (usedQueueSize < (NUM_STAGES - STAGE_EXECUTE)) {
    throw PipelineHazardException("Failed to store result for RD after execution, size of the pipeline queue is not sufficient");
  }

  executingQueue[STAGE_EXECUTE].RDVal = result;
}

void PipelineHazardController::storeResultAfterMemoryAccess(bytes result) {
  // Updates the result entry of a corresponding instruction entry in the pipeline
  // used after loads to pass the value back to any decode stages
  lock_guard<mutex> lck(lock);
  if (result.size() == 0 || result.size() != XLEN) {
    throw PipelineHazardException("Failed to store result for RD after execution, result is wrong size");
  }

  if (usedQueueSize < (NUM_STAGES - STAGE_MEM)) {
    throw PipelineHazardException("Failed to store result for RD after execution, size of the pipeline queue is not sufficient");
  }

  executingQueue[STAGE_MEM].RDVal = result;
}
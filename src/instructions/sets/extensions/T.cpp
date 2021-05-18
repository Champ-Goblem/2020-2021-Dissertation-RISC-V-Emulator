#include "../../../include/instructions/sets/extensions/T.h"
#include "../../../include/exceptions.h"
#include "../../../include/instructions/AbstractInstruction.h"
#include "../../../include/instructions/IType.h"
#include "../../../include/instructions/RType.h"
#include "../../../include/units/PipelineHazardController.h"
#include "../../../include/hw/Memory.h"
#include "../../../include/hw/RegisterFile.h"

AbstractInstruction TExtension::decode(bytes instruction, PipelineHazardController* pipelineController, bool* stall) {
  // I-Type
  ITypeInstruction ins = ITypeInstruction(pipelineController->getXLEN());
  ins.decode(instruction);
  ins.execute = &execute;
  ins.memoryAccess = nullptr;
  ins.registerWriteback = &writeback;
  if (pipelineController->checkForStaleRegister(ins.getRS1())) {
    pipelineController->enqueue(NOP(pipelineController->getXLEN()));
    *stall = true;
    return NOP(pipelineController->getXLEN());
  } else {
    ins.setRs1Val(pipelineController->fetchRegisterValue(ins.getRS1()));
  }
  pipelineController->enqueue(ins);
  return ins;
}

void TExtension::execute(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize, PipelineHazardController* pipelineController) {
  if (instruction->getType() != InstructionType::I) {
    throw UndefinedInstructionException(instruction, "Instruction not I-Type as expected");
  }

  if (instruction->getFunc3() == 0) {
    instruction->setResult(addByteToBytes(instruction->getRs1Val(), 1));
  } else if (instruction->getFunc3() == 1) {
    instruction->setResult(addByteToBytes(instruction->getRs1Val(), -1));
  } else {
    throw UndefinedInstructionException(instruction, "Func3 undefined");
  }

  pipelineController->storeResultAfterExecution(instruction->getResult());
}

void TExtension::writeback(AbstractInstruction* instruction, RegisterFile* registerFile) {
  registerFile->write(instruction->getRD(), instruction->getResult());
}
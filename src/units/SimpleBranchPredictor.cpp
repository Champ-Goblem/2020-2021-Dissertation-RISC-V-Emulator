#include "../include/units/SimpleBranchPredictor.h"
#include "../include/exceptions.h"
#include "../include/hw/Memory.h"
#include "../include/hw/RegisterFile.h"
#include "../include/instructions/BType.h"
#include "../include/instructions/IType.h"
#include "../include/instructions/JType.h"

SimpleBranchPredictor::SimpleBranchPredictor(Memory* memory, ushort XLEN, RegisterFile* registerFile, bytes initialPC) {
  if (initialPC.size() != XLEN) {
    throw BranchPredictorException("Initial PC invalid, not size of XLEN [%s]", getBytesForPrint(initialPC).c_str());
  }

  ulong pcVal = getBytesToULong(initialPC);
  if (pcVal >= memory->getSize()) {
    throw AddressOutOfMemoryException(pcVal, 4, memory->getSize(), true, "Initial PC invalid, points to out of memory");
  }

  if (initialPC[0] % 4 != 0) {
    throw AddressMisalignedException(initialPC, bytes{0}, bytes{0}, "Initial PC invalid, not 4 byte aligned");
  }

  this->memory = memory;
  this->XLEN = XLEN;
  this->registerFile = registerFile;

  this->PCQueue.push(initialPC);

  this->workloop = thread (&SimpleBranchPredictor::predictionWorkloop, this);
  // this->workloop->detach();
}

// SimpleBranchPredictor::SimpleBranchPredictor(SimpleBranchPredictor&& obj): workloop(move(obj.workloop)) {};

// SimpleBranchPredictor& SimpleBranchPredictor::operator=(SimpleBranchPredictor&& obj) {
//   if (workloop->joinable()) {
//     workloop->join();
//   }

//   workloop = move(obj.workloop);
//   return *this;
// }

bytes SimpleBranchPredictor::getNextPC() {
  lock_guard<mutex> lck(lock);
  if (this->workloop.joinable()) this->workloop.join();
  // delete(this->workloop);
  // Check that no exceptions have been thrown by the thread
  if (this->workloopExceptionPtr) {
    rethrow_exception(this->workloopExceptionPtr);
  }

  // Check that the queue size isnt zero
  if (this->PCQueue.size() < 2) {
    // TODO: decide what to do here
  }

  if (this->executingQueue.size() > 2) {
    this->executingQueue.pop();
  }

  // Get the next PC from the queue
  bytes nextPC = this->PCQueue.front();
  // Drop the front of the queue
  this->PCQueue.pop();
  // Add this to the queue of waiting for executionlock_guard
  this->executingQueue.push(PCQueue.front());
  // Start new thread to fetch new PC
  this->workloop = thread (&SimpleBranchPredictor::predictionWorkloop, this);
  // this->workloop->detach();
  return nextPC;
}

bool SimpleBranchPredictor::checkPrediction(bytes pc, bytes address) {
  lock_guard<mutex> lck(lock);
  if (this->workloop.joinable()) this->workloop.join();
  if (address.size() == 0) {
    throw BranchPredictorException("Provided address to check prediction is zero bytes in length");
  }

  if (this->executingQueue.size() == 0) {
    throw BranchPredictorException("Failed to fetch prediction for next PC, executing queue empty");
  }

  // Get the first most value in the queue, this should be next instruction
  // to execute as predicted by the bp
  bytes nextPredicted = this->executingQueue.front();
  // Pop this value off now we are dealing with it
  this->executingQueue.pop();
  // Verify if we predicted correctly
  if (nextPredicted == address) {
    return true;
  }

  // Fix the mistake here
  // Cancel any more prediction
  this->failedPrediction = true;
  // this->workloop.join();
  // delete(this->workloop);
  while (!this->PCQueue.empty()) {
    this->PCQueue.pop();
  }
  this->PCQueue.push(address);
  // Start fetching again
  this->failedPrediction = false;
  this->workloop = thread(&SimpleBranchPredictor::predictionWorkloop, this);
  // this->workloop->detach();
  return false;
}

void SimpleBranchPredictor::predictionWorkloop() {
  // lock_guard<mutex> lck(lock);
  bool exception = false;
  try {
    // Check that the initial size of the queue isnt zero
    // The queue should have at least one value in it
    if (this->PCQueue.size() == 0) {
      throw BranchPredictorException("Failed to start prediction, queue length 0\n");
    }
    
    if (this->PCQueue.back()[0] % 4 != 0) {
      throw BranchPredictorException("Failed to start prediction, inital PC not 4-bytes aligned\n");
    }
  } catch (...) {
    // Pass the current exception to the exception ptr;
    this->workloopExceptionPtr = current_exception();
    exception = true;
  }

  while (!exception && !this->isProcessorExceptionGenerated && !this->failedPrediction && this->PCQueue.size() < this->queueSize) {
    bytes nextPC;
    try {
      // TODO: Handle address-misaligned exceptions
      bytes lastPC = this->PCQueue.back();
      // cout << "lpc: " << getBytesForPrint(lastPC) << "\n";
      bytes instruction = this->memory->readWord(getBytesToULong(lastPC));
      // cout << "inst: " << getBytesForPrint(instruction) << "\n";
      byte opcode = getContrainedBits(instruction, 0, 6)[0];
      if (opcode == 99) {
        // If opcode = beq | bne | blt | bge | bltu | bgeu
        // then we need to check if the sign bit of the imm is negative,
        // if so take PC - imm else PC + 4
        // Uses B-Type
        BTypeInstruction b = BTypeInstruction(XLEN);
        b.decode(instruction);
        bytes imm = b.AbstractInstruction::getImm();
        nextPC = bytesAddSignedToPC(lastPC, imm);
        if (instruction[3] >> 7 != 1 || nextPC[0] % 4 != 0) {
          nextPC = addByteToBytes(lastPC, 4);
        }

      } else if (opcode == 103) {
        // Opcode = JALR
        // Uses I-Type
        ITypeInstruction i = ITypeInstruction(XLEN);
        i.decode(instruction);
        bytes imm = i.AbstractInstruction::getImm();
        bytes rs1Val = this->registerFile->get((ushort)i.getRS1());
        nextPC = bytesAddSignedToPC(rs1Val, imm);
      } else if (opcode == 111) {
        // opcode - JAL
        // Uses J-Type
        JTypeInstruction j = JTypeInstruction(XLEN);
        j.decode(instruction);
        bytes imm = j.AbstractInstruction::getImm();
        nextPC = bytesAddSignedToPC(lastPC, imm);
      } else {
        nextPC = addByteToBytes(lastPC, 4);
      }

      ulong pcVal = getBytesToULong(nextPC);
      if (pcVal >= this->memory->getSize()) {
        throw AddressOutOfMemoryException(pcVal, 4, this->memory->getSize(), true);
      }
      if (nextPC[0] % 4 != 0) {
        throw BranchPredictorException("Generated PC not 4 bytes aligned [%s]", getBytesForPrint(nextPC).c_str());
      }
    } catch (...) {
      this->workloopExceptionPtr = current_exception();
      exception = true;
    }

    // Push the calculated to the queue so long as
    // No exception generated
    // No public exception generated
    // No failed prediction
    if (!exception && !this->isProcessorExceptionGenerated && !this->failedPrediction) {
      // Ensure we dont push a value to the queue while the other thread is removing a value
      this->PCQueue.push(nextPC);
    }
  }
}

bytes SimpleBranchPredictor::peak() {
  lock_guard<mutex> lck(lock);
  return this->PCQueue.front();
}

SimpleBranchPredictor::~SimpleBranchPredictor() {
  this->isProcessorExceptionGenerated = true;
  if (workloop.joinable()) {
    this->workloop.join();
  }
  // delete(this->workloop);
}
#include "../include/units/SimpleBranchPredictor.h"

SimpleBranchPredictor::SimpleBranchPredictor(Memory* memory, ushort XLEN, RegisterFile* registerFile, bytes initialPC) {
  if (initialPC.size() != XLEN) {
    throw new BranchPredictorException("Initial PC invalid, not size of XLEN [%s]", getBytesForPrint(initialPC).c_str());
  }

  ulong pcVal = getBytesToULong(initialPC);
  if (pcVal >= memory->getSize()) {
    throw new BranchPredictorException("Initial PC invalid, points to out of memory [PC: %d, max: %d]", pcVal, memory->getSize());
  }

  if (initialPC[0] % 4 != 0) {
    throw new BranchPredictorException("Initial PC invalid, not 4 byte aligned [%s]", getBytesForPrint(initialPC).c_str());
  }

  this->memory = memory;
  this->XLEN = XLEN;
  this->registerFile = registerFile;

  this->PCQueue.push(initialPC);

  this->workloop = thread (&SimpleBranchPredictor::predictionWorkloop, this);
}

bytes SimpleBranchPredictor::getNextPC() {
  // Check that no exceptions have been thrown by the thread
  if (this->workloopExceptionPtr) {
    rethrow_exception(this->workloopExceptionPtr);
  }

  // Check that the queue size isnt zero
  if (this->PCQueue.size() == 0) {
    // TODO: decide what to do here
  }

  // Get the next PC from the queue
  bytes nextPC = this->PCQueue.front();
  // Ensure that we dont pop a value off while the other thread is accessing the queue
  unique_lock<mutex> queueLock(queueMutex);
  // Drop the front of the queue
  this->PCQueue.pop();
  queueLock.unlock();
  // Notify the thread if paused that a new PC has been taken
  queueCV.notify_all();
  return nextPC;
}

bool SimpleBranchPredictor::checkPrediction(bytes pc, bytes address) {
  return true;
}

void SimpleBranchPredictor::predictionWorkloop() {
  bool exception;
  try {
    // Check that the initial size of the queue isnt zero
    // The queue should have at least one value in it
    if (this->PCQueue.size() == 0) {
      throw new BranchPredictorException("Failed to start prediction, queue length 0");
    }

    if (this->PCQueue.back()[0] % 4 != 0) {
      throw new BranchPredictorException("Failed to start prediction, inital PC not 4-bytes aligned [%s]", getBytesForPrint(this->PCQueue.back()).c_str());
    }
  } catch (...) {
    // Pass the current exception to the exception ptr;
    this->workloopExceptionPtr = current_exception();
    exception = true;
  }

  // Needed for conditional_variable
  mutex mutexForCV;
  unique_lock<mutex> mutexLock(mutexForCV);
  while (!exception && !this->isProcessorExceptionGenerated) {
    bytes nextPC;
    try {
      // TODO: Handle address-misaligned exceptions
      bytes lastPC = PCQueue.back();
      bytes instruction = memory->readDWord(getBytesToULong(lastPC));
      byte opcode = getContrainedBits(instruction, 0, 6)[0];
      if (opcode == 99) {
        // If opcode = beq | bne | blt | bge | bltu | bgeu
        // then we need to check if the sign bit of the imm is negative,
        // if so take PC - imm else PC + 4
        // Uses B-Type
        if (instruction[3] >> 7 == 1) {
          BTypeInstruction b = BTypeInstruction();
          b.decode(instruction);
          bytes imm = b.AbstractInstruction::getImm();
          imm[1] &= 15;
          nextPC = subBytesFromBytes(lastPC, imm);
        } else {
          nextPC = addByteToBytes(lastPC, 4);
        }

      } else if (opcode == 103) {
        // Opcode = JALR
        // Uses I-Type
        ITypeInstruction i = ITypeInstruction();
        i.decode(instruction);
        bytes imm = i.AbstractInstruction::getImm();
        imm[1] &= 3;
        bytes rs1Val = this->registerFile->get((ushort)i.getRS1());
        if (instruction[3] >> 7 == 0) {
          nextPC = addBytesToBytes(rs1Val, imm);
        } else {
          nextPC = subBytesFromBytes(rs1Val, imm);
        }
      } else if (opcode == 111) {
        // opcode - JAL
        // Uses J-Type
        JTypeInstruction j = JTypeInstruction();
        j.decode(instruction);
        bytes imm = j.AbstractInstruction::getImm();
        imm[2] &= 15;
        if (instruction[3] >> 7 == 0) {
          nextPC = addBytesToBytes(lastPC, imm);
        } else {
          nextPC = subBytesFromBytes(lastPC, imm);
        }
      }

      // TODO: This needs changing to something that supports more than 8 bytes
      ulong pcVal = getBytesToULong(nextPC);
      if (pcVal >= memory->getSize()) {
        throw new BranchPredictorException("Generated PC invalid, points to out of memory [PC: %d, max: %d]", pcVal, memory->getSize());
      }
      if (nextPC[0] % 4 != 0) {
        throw new BranchPredictorException("Generated PC not 4 bytes aligned [%s]", getBytesForPrint(nextPC).c_str());
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
      unique_lock<mutex> queueLock(queueMutex);
      this->PCQueue.push(nextPC);
      queueLock.unlock();
    }

    // Wait for signal if queue size is equal to X or we failed a prediction so longs as no public exception generated
    if ((this->PCQueue.size() == this->queueSize || this->failedPrediction) && !this->isProcessorExceptionGenerated) printf("Prediction workloop waiting for signal...");
    this->queueCV.wait(mutexLock, [this]{return (this->PCQueue.size() == this->queueSize || this->failedPrediction) && !this->isProcessorExceptionGenerated;});
    printf("Prediction workloop resuming...");
  }
  mutexLock.unlock();
}
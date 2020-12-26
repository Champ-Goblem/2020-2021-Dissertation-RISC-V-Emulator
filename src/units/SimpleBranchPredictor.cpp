#include "../include/units/SimpleBranchPredictor.h"

SimpleBranchPredictor::SimpleBranchPredictor(Memory* memory, ushort XLEN, bytes initialPC): AbstractBranchPredictor(memory, XLEN) {
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
  // Drop the front of the queue
  this->PCQueue.pop();
  // Notify the thread if paused that a new PC has been taken
  queueCV.notify_all();
  return nextPC;
}

void SimpleBranchPredictor::predictionWorkloop() {
  bool exception;
  try {
    // Check that the initial size of the queue isnt zero
    // The queue should have at least one value in it
    if (this->PCQueue.size() == 0) {
      throw new BranchPredictorException("Failed to start prediction, queue length 0");
    }
  } catch (...) {
    // Pass the current exception to the exception ptr;
    this->workloopExceptionPtr = current_exception();
    exception = true;
  }

  // Lock the mutex, needed for conditional_variable
  unique_lock<mutex> mutexLock(queueMutex);
  while (!exception && !this->isProcessorExceptionGenerated) {
    bytes nextPC;
    try {
      // Calculate PC+4
      nextPC = addByteToBytes(this->PCQueue.back(), 4);

      // TODO: This needs changing to something that supports more than 8 bytes
      ulong pcVal = getBytesToULong(nextPC);
      if (pcVal >= memory->getSize()) {
        throw new BranchPredictorException("Initial PC invalid, points to out of memory [PC: %d, max: %d]", pcVal, memory->getSize());
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
      this->PCQueue.push(nextPC);
    }

    // Wait for signal if queue size is equal to X or we failed a prediction so longs as no public exception generated
    if ((this->PCQueue.size() == this->queueSize || this->failedPrediction) && !this->isProcessorExceptionGenerated) {
      this->queueCV.wait(mutexLock);
    }
  }
  mutexLock.unlock();
}
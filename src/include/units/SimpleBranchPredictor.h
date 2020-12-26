#ifndef __SIMPLEBRANCHPREDICTOR__
#define __SIMPLEBRANCHPREDICTOR__

#include <queue>
#include <mutex>
#include <condition_variable>

#include "AbstractBranchPredictor.h"

class SimpleBranchPredictor: public AbstractBranchPredictor {
  private:
  queue<bytes> PCQueue;
  thread workloop;
  bool failedPrediction;
  exception_ptr workloopExceptionPtr;
  const ushort queueSize = 4;
  mutex queueMutex;
  condition_variable queueCV;

  public:
  SimpleBranchPredictor(Memory* memory, ushort XLEN, bytes initialPC);
  bytes getNextPC();

  private:
  void predictionWorkloop();
};

#endif
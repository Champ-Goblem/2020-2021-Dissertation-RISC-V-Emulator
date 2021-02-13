#ifndef __SIMPLEBRANCHPREDICTOR__
#define __SIMPLEBRANCHPREDICTOR__

#include <queue>

#include "AbstractBranchPredictor.h"

class SimpleBranchPredictor: public AbstractBranchPredictor {
  private:
  queue<bytes> PCQueue;
  thread workloop;
  bool failedPrediction = false;
  exception_ptr workloopExceptionPtr;
  const ushort queueSize = 4;
  queue<bytes> executingQueue;

  public:
  SimpleBranchPredictor(Memory* memory, ushort XLEN, RegisterFile* registerFile, bytes initialPC);
  bytes getNextPC();
  bool checkPrediction(bytes pc, bytes address);
  bytes peak();
  ~SimpleBranchPredictor();

  private:
  void predictionWorkloop();
};

#endif
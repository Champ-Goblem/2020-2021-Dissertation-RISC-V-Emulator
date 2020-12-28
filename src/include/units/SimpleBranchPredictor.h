#ifndef __SIMPLEBRANCHPREDICTOR__
#define __SIMPLEBRANCHPREDICTOR__

#include <queue>
#include <mutex>
#include <condition_variable>

#include "AbstractBranchPredictor.h"
#include "../instructions/BType.h"
#include "../instructions/IType.h"
#include "../instructions/JType.h"

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
  SimpleBranchPredictor(Memory* memory, ushort XLEN, RegisterFile* registerFile, bytes initialPC);
  bytes getNextPC();
  bool checkPrediction(bytes pc, bytes address);

  private:
  void predictionWorkloop();
};

#endif
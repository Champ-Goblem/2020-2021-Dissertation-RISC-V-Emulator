#ifndef __ABSTRACTBRANCHPREDICTOR__
#define __ABSTRACTBRANCHPREDICTOR__

#include "../emustd.h"
#include "../hw/Memory.h"

class AbstractBranchPredictor {
  protected:
  Memory* memory;
  public:
  AbstractBranchPredictor();
  bytes getNextPC();
  bool checkPrediction(bytes pc, bytes addr);
};

#endif
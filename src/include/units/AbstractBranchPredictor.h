#ifndef __AbstractBranchPredictor__
#define __AbstractBranchPredictor__

#include "../emustd.h"
#include "../hw/Memory.h"
#include "AbstractUnit.h"

class BranchPredictorException: public EmulatorException {
  public:
  BranchPredictorException(const char* message, ...): EmulatorException(message){};
};

class AbstractBranchPredictor: public AbstractUnit {
  protected:
  Memory* memory;
  ushort XLEN;

  public:
  AbstractBranchPredictor(Memory* memory, ushort XLEN);
  virtual bytes getNextPC();
  virtual bool checkPrediction(bytes pc, bytes addr);
};

#endif
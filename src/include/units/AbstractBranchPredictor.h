#ifndef __AbstractBranchPredictor__
#define __AbstractBranchPredictor__

#include "../emustd.h"
#include "../exceptions.h"
#include "../hw/Memory.h"
#include "AbstractUnit.h"
#include "../hw/RegisterFile.h"

class BranchPredictorException: public EmulatorException {
  public:
  BranchPredictorException(const char* message, ...): EmulatorException(message){};
};

class AbstractBranchPredictor: public AbstractUnit {
  protected:
  Memory* memory;
  RegisterFile* registerFile;
  ushort XLEN;

  public:
  virtual bytes getNextPC(){};
  virtual bool checkPrediction(bytes pc, bytes addr){};
};

#endif
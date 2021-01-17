#ifndef __AbstractBranchPredictor__
#define __AbstractBranchPredictor__

#include "../emustd.h"
#include "AbstractUnit.h"
// #include "../hw/Memory.h"
// #include "../hw/RegisterFile.h"

class Memory;
class RegisterFile;

class BranchPredictorException: public EmulatorException {
  public:
  BranchPredictorException(string message, ...): EmulatorException(message){};
};


class AbstractBranchPredictor: public AbstractUnit {
  protected:
  Memory* memory;
  RegisterFile* registerFile;
  ushort XLEN;

  public:
  virtual bytes getNextPC(){ return bytes(0); };
  virtual bool checkPrediction(bytes pc, bytes addr){ return false; };
};

#endif
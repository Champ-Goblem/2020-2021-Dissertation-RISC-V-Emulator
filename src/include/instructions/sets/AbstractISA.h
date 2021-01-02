#ifndef __AbstractISA__
#define __AbstractISA__

#include "../../emustd.h"
#include "../../exceptions.h"
#include "../AbstractInstruction.h"

struct OpcodeSpace {
  ushort opcode;
  AbstractInstruction (*decodeRoutine)(bytes);
};

class AbstractISA {
  public:
  virtual vector<OpcodeSpace> registerOpcodeSpace() { return vector<OpcodeSpace>(0); };
  
  // Decode routines should be declared private and have a signature of
  // AbstractInstruction decodeSomething(bytes instruction) {...};
};

#endif
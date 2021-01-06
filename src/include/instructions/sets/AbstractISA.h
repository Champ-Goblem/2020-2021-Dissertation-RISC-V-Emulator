#ifndef __AbstractISA__
#define __AbstractISA__

#include "../../emustd.h"
#include "../../exceptions.h"
#include "../AbstractInstruction.h"
#include "../BType.h"
#include "../IType.h"
#include "../JType.h"
#include "../RType.h"
#include "../SType.h"
#include "../UType.h"

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
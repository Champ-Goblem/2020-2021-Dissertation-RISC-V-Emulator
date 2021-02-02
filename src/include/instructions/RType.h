#ifndef __RTYPE__
#define __RTYPE__

#include "AbstractInstruction.h"

#define NOP RTypeInstruction(0, 51, 0, 0, 0, 0, 0);
class RTypeInstruction: public AbstractInstruction {
  
  public:
  RTypeInstruction(ushort XLEN);
  RTypeInstruction(ushort XLEN, byte opcode, byte rd, byte func3, byte rs1, byte rs2, byte funct7);

  void decode(bytes instruction);
  string debug();
};

#endif
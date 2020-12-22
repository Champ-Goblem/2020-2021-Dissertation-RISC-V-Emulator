#ifndef __STYPE__
#define __STYPE__

#include "AbstractInstruction.h"

#define IMM5_MAX 31
#define IMM7_MAX 127

class STypeInstruction: public AbstractInstruction {
  
  public:
  STypeInstruction();
  STypeInstruction(byte opcode, byte imm5, byte func3, byte rs1, byte rs2, byte imm7);

  void decode(bytes instruction);
  bytes getImm(ushort low, ushort high);
};

#endif
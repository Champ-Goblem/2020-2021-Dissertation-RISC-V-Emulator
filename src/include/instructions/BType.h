#ifndef __BTYPE__
#define __BTYPE__

#include "AbstractInstruction.h"

#define IMM1_MAX 1
#define IMM4_MAX 15
#define IMM6_MAX 63

class BTypeInstruction: public AbstractInstruction {
  
  public:
  BTypeInstruction(ushort XLEN);
  BTypeInstruction(ushort XLEN, byte opcode, byte imm1, byte imm4, byte func3, byte rs1, byte rs2, byte imm6, byte imm31);

  void decode(bytes instruction);
  bytes getImm(ushort low, ushort high);
  string debug();
};

#endif
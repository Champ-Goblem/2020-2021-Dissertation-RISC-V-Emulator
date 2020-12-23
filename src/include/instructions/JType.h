#ifndef __JTYPE__
#define __JTYPE__

#include "AbstractInstruction.h"

#define IMM8_MAX 255
#define IMM1_MAX 1
#define IMM10_MAX 1023
#define IMM10_SIZE 2

class JTypeInstruction: public AbstractInstruction {
  
  public:
  JTypeInstruction();
  JTypeInstruction(byte opcode, byte rd, byte imm8, byte imm1, bytes imm10, byte imm31);

  void decode(bytes instruction);
  bytes getImm(ushort low, ushort high);
};

#endif
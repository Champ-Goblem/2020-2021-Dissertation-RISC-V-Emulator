#ifndef __ITYPE__
#define __ITYPE__

#include "AbstractInstruction.h"

#define MAX_IMM_SIZE 4095

class ITypeInstruction: public AbstractInstruction {
  
  public:
  ITypeInstruction();
  ITypeInstruction(byte opcode, byte rd, byte func3, byte rs1, bytes imm);

  void decode(bytes instruction);
};

#endif
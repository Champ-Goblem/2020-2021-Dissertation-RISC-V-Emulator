#ifndef __UTYPE__
#define __UTYPE__

#include "AbstractInstruction.h"

#define IMM_MAX 1048575
#define IMM_SIZE 3

class UTypeInstruction: public AbstractInstruction {
  
  public:
  UTypeInstruction(ushort XLEN);
  UTypeInstruction(ushort XLEN, byte opcode, byte rd, bytes imm);

  void decode(bytes instruction);
  bytes getImm(ushort low, ushort high);
  string debug();
};

#endif
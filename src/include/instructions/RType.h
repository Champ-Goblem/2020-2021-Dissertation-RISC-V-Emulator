#ifndef __RTYPE__
#define __RTYPE__

#include "AbstractInstruction.h"

class RTypeInstruction: public AbstractInstruction {
  
  public:
  RTypeInstruction();
  RTypeInstruction(byte opcode, byte rd, byte func3, byte rs1, byte rs2, byte funct7);

  void decode(bytes instruction);
};

#endif
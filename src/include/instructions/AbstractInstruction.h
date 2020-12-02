#ifndef __ABSTRACTINSTRUCTION__
#define __ABSTRACTINSTRUCTION__

#include "../emustd.h"

class AbstractInstruction {
  protected:
  byte opcode;
  byte rs1;
  byte rs2;
  byte rd;
  byte func3;
  byte func7;
  bytes imm = bytes (4);
  
  public:
  AbstractInstruction(bytes inst);
  byte getOpcode();
  byte getRS1();
  byte getRS2();
  byte getRD();
  byte getFunc3();
  byte getFunc7();
  bytes getImm();
  bytes getImm(ushort low, ushort high);
};

#endif
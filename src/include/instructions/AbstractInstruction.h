#ifndef __ABSTRACTINSTRUCTION__
#define __ABSTRACTINSTRUCTION__

#include "../emustd.h"
#include "../bytemanip.h"

class AbstractInstructionException: public exception {
  private:
  const char* message;

  public:
  AbstractInstructionException() {}
  AbstractInstructionException(const char* message, ...) {
    this->message = message;
  }
  const char* getMessage() {
    return message;
  }
};

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
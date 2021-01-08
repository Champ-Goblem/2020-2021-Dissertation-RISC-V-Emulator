#ifndef __ABSTRACTINSTRUCTION__
#define __ABSTRACTINSTRUCTION__

#include "../emustd.h"
#include "../bytemanip.h"
#include "../hw/RegisterFile.h"
#include "../hw/Memory.h"

#define OPCODE_MAX 127
#define R_MAX 31
#define FUNC3_MAX 7
#define FUNC7_MAX 127
#define INSTRUCTION_SIZE 4

enum InstructionType {
  R,
  I,
  S,
  B,
  U,
  J
};

class InstructionException: public exception {
  private:
  const char* message;

  public:
  InstructionException() {}
  InstructionException(const char* message, ...) {
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
  bytes imm;
  InstructionType type;
  bytes rs1Val;
  bytes rs2Val;
  bytes result;
  bytes pc;
  bool isSigned;

  public:
  byte getOpcode();
  byte getRS1();
  byte getRS2();
  byte getRD();
  byte getFunc3();
  byte getFunc7();
  bytes getImm();
  bytes getImm(ushort low, ushort high);
  InstructionType getType();
  bytes getRs1Val();
  void setRs1Val(bytes val);
  bytes getRs2Val();
  void setRs2Val(bytes val);
  bytes getResult();
  void setResult(bytes val);
  bytes getPC();
  void setPC(bytes val);
  bool isSignedImmediate();
  virtual string debug() {};
  void (*execute)(AbstractInstruction* instruction);
  void (*registerWriteback)(AbstractInstruction* instruction, RegisterFile* registerFile);
  void (*memoryAccess)(AbstractInstruction* instruction, Memory* memory);
};

#endif
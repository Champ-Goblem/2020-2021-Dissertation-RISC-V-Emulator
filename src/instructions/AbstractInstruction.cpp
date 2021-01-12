#include "../include/instructions/AbstractInstruction.h"

byte AbstractInstruction::getOpcode() {
  return opcode;
};

byte AbstractInstruction::getRS1() {
  return rs1;
};

byte AbstractInstruction::getRS2() {
  return rs2;
};

byte AbstractInstruction::getRD() {
  return rd;
};

byte AbstractInstruction::getFunc3() {
  return func3;
};

byte AbstractInstruction::getFunc7() {
  return func7;
};

bytes AbstractInstruction::getImm() {
  return imm;
};

bytes AbstractInstruction::getImm(ushort low, ushort high) {
  throw InstructionException("Failed to get imm, does not exist on this instruction type\n");
};

InstructionType AbstractInstruction::getType() {
  return type;
}

bytes AbstractInstruction::getRs1Val() {
  return rs1Val;
}

void AbstractInstruction::setRs1Val(bytes val) {
  this->rs1Val = val;
}

bytes AbstractInstruction::getRs2Val() {
  return rs2Val;
}

void AbstractInstruction::setRs2Val(bytes val) {
  this->rs2Val = val;
}

bytes AbstractInstruction::getResult() {
  return result;
}

void AbstractInstruction::setResult(bytes result) {
  this->result = result;
}

bytes AbstractInstruction::getPC() {
  return pc;
}

void AbstractInstruction::setPC(bytes pc) {
  this->pc = pc;
}

bool AbstractInstruction::isSignedImmediate() {
  return this->isSigned;
}

ushort AbstractInstruction::getXLEN() {
  return this->XLEN;
}
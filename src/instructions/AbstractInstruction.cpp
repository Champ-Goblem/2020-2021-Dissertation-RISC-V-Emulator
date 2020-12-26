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
  throw new InstructionException("Failed to get imm, does not exist on this instruction type");
};

InstructionType AbstractInstruction::getType() {
  return type;
}
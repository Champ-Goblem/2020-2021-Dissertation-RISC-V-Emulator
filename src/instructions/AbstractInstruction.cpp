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

// 0  3 4  7   8  1  2  5   6  9 0  3   4   7 8  1
// 0000 0000   0000 |1000   1110 1000   001|0 0000
//                   1000   1110 1000   0010
bytes AbstractInstruction::getImm(ushort low, ushort high) {
  try {
    return getContrainedBits(this->imm, low, high);
  } catch (EmulatorException e) {
    throw new AbstractInstructionException(e.getMessage());
  }
};
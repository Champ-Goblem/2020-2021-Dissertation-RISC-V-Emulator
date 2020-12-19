#include "../include/instructions/RType.h"

RTypeInstruction::RTypeInstruction() {
  this->opcode = 0;
  this->rs1 = 0;
  this->rs2 = 0;
  this->rd = 0;
  this->func3 = 0;
  this-> func7 = 0;
  this->imm = bytes(4);
}

RTypeInstruction::RTypeInstruction(byte opcode, byte rd, byte func3, byte rs1, byte rs2, byte funct7) {
  if (opcode > 127) {
    throw new AbstractInstructionException("Failed to set opcode, greater than 63 [%d]", opcode);
  }
  this->opcode = opcode;
  if (rd > 31) {
    throw new AbstractInstructionException("Failed to set rd, greater than 31 [%d]", rd);
  }
  this->rd = rd;
  if (func3 > 7) {
    throw new AbstractInstructionException("Failed to set func3, greater than 63 [%d]", func3);
  }
  this->func3 = func3;
  if (rs1 > 31) {
    throw new AbstractInstructionException("Failed to set rs1, greater than 63 [%d]", rs1);
  }
  this->rs1 = rs1;
  if (rs2 > 31) {
    throw new AbstractInstructionException("Failed to set rs2, greater than 63 [%d]", rs2);
  }
  this->rs2 = rs2;
  if (func7 > 127) {
    throw new AbstractInstructionException("Failed to set func7, greater than 63 [%d]", func7);
  }
  this->func7 = func7;
}
void RTypeInstruction::decode(bytes instruction) {
  if (instruction.size() != 4) {
    throw new AbstractInstructionException("Failed to decode instruction, instructions not 4 bytes");
  }
  try {
    this->opcode = getContrainedBits(instruction, 0, 6)[1];
    this->rd = getContrainedBits(instruction, 7, 11)[1];
    this->func3 = getContrainedBits(instruction, 12, 14)[1];
    this->rs1 = getContrainedBits(instruction, 15, 19)[1];
    this->rs2 = getContrainedBits(instruction, 20, 24)[1];
    this->func7 = getContrainedBits(instruction, 25, 31)[1];
  } catch (exception e) {
    throw e;
  }

}
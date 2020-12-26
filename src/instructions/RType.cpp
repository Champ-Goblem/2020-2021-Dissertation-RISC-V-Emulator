#include "../include/instructions/RType.h"

RTypeInstruction::RTypeInstruction() {
  this->opcode = 0;
  this->rs1 = 0;
  this->rs2 = 0;
  this->rd = 0;
  this->func3 = 0;
  this->func7 = 0;
  this->imm = bytes(4);
  this->type = InstructionType::R;
}

RTypeInstruction::RTypeInstruction(byte opcode, byte rd, byte func3, byte rs1, byte rs2, byte func7) {
  if (opcode > OPCODE_MAX) {
    throw new InstructionException("Failed to set opcode, greater than 63 [%d]", opcode);
  }
  this->opcode = opcode;

  if (rd > R_MAX) {
    throw new InstructionException("Failed to set rd, greater than 31 [%d]", rd);
  }
  this->rd = rd;

  if (func3 > FUNC3_MAX) {
    throw new InstructionException("Failed to set func3, greater than 7 [%d]", func3);
  }
  this->func3 = func3;

  if (rs1 > R_MAX) {
    throw new InstructionException("Failed to set rs1, greater than 31 [%d]", rs1);
  }
  this->rs1 = rs1;

  if (rs2 > R_MAX) {
    throw new InstructionException("Failed to set rs2, greater than 31 [%d]", rs2);
  }
  this->rs2 = rs2;
  
  if (func7 > FUNC7_MAX) {
    throw new InstructionException("Failed to set func7, greater than 127 [%d]", func7);
  }
  this->func7 = func7;

  this->type = InstructionType::R;
}

void RTypeInstruction::decode(bytes instruction) {
  if (instruction.size() != INSTRUCTION_SIZE) {
    throw new InstructionException("Failed to decode instruction, not 4 bytes");
  }
  try {
    this->opcode = getContrainedBits(instruction, 0, 6)[0];
    this->rd = getContrainedBits(instruction, 7, 11)[0];
    this->func3 = getContrainedBits(instruction, 12, 14)[0];
    this->rs1 = getContrainedBits(instruction, 15, 19)[0];
    this->rs2 = getContrainedBits(instruction, 20, 24)[0];
    this->func7 = getContrainedBits(instruction, 25, 31)[0];
  } catch (exception e) {
    throw e;
  }

  this->type = InstructionType::R;
}
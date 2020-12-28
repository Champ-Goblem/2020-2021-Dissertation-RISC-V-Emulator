#include "../include/instructions/SType.h"

STypeInstruction::STypeInstruction() {
  this->opcode = 0;
  this->rd = 0;
  this->func3 = 0;
  this->rs1 = 0;
  this->rs2 = 0;
  this->imm = bytes(2);
  this->type = InstructionType::S;
}

STypeInstruction::STypeInstruction(byte opcode, byte imm5, byte func3, byte rs1, byte rs2, byte imm7) {
  this->imm = bytes(2);
  
  if (opcode > OPCODE_MAX) {
    throw new InstructionException("Failed to set opcode, greater than 63 [%d]", opcode);
  }
  this->opcode = opcode;

  if (imm5 > IMM5_MAX) {
    throw new InstructionException("Failed to set imm5, greater than 31 [%d]", imm5);
  }
  this->imm[0] = imm5;

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

  if (imm7 > IMM7_MAX) {
    throw new InstructionException("Failed to set imm7, greater than 127 [%d]", imm7);
  }
  this->imm[0] |= imm7 << 5;
  this->imm[1] |= imm7 >> 3;

  this->type = InstructionType::S;
}

void STypeInstruction::decode(bytes instruction) {
  if (instruction.size() != INSTRUCTION_SIZE) {
    throw new InstructionException("Failed to decode instruction, not 4 bytes in length [%d]", instruction.size());
  }

  try {
    this->opcode = getContrainedBits(instruction, 0, 6)[0];
    this->imm[0] = getContrainedBits(instruction, 7, 11)[0];
    this->func3 = getContrainedBits(instruction, 12, 14)[0];
    this->rs1 = getContrainedBits(instruction, 15, 19)[0];
    this->rs2 = getContrainedBits(instruction, 20, 24)[0];
    byte imm7 = getContrainedBits(instruction, 25, 31)[0];
    this->imm[0] |= imm7 << 5;
    this->imm[1] = imm7 >> 3;
  } catch (exception e) {
    throw (e);
  }

  this->type = InstructionType::S;
}

bytes STypeInstruction::getImm(ushort low, ushort high) {
  if (low == 7 && high == 11) {
    bytes imm = bytes(1);
    imm[0] = this->imm[0] & 31;
    return imm;
  } else if (low == 25 && high == 31) {
    bytes imm = bytes(1);
    imm[0] = this->imm[0] >> 5;
    imm[0] |= this->imm[1] << 3;
    return imm;
  }
  
  throw new InstructionException("Failed to get imm, does not exist in this instruction type [low: %d, high: %d]", low, high);
}
#include "../include/instructions/JType.h"

JTypeInstruction::JTypeInstruction() {
  this->opcode = 0;
  this->rd = 0;
  this->imm = bytes(3);
  this->type = InstructionType::J;
}

JTypeInstruction::JTypeInstruction(byte opcode, byte rd, byte imm8, byte imm1, bytes imm10, byte imm31) {
  this->imm = bytes(3);

  if (opcode > OPCODE_MAX) {
    throw new InstructionException("Failed to set opcode, greater than 63 [%d]\n", opcode);
  }
  this->opcode = opcode;

  if (rd > R_MAX) {
    throw new InstructionException("Failed to set rd, greater than 31 [%d]\n", rd);
  }
  this->rd = rd;

  if (imm8 > IMM8_MAX) {
    throw new InstructionException("Failed to set imm8, greater than 255 [%d]\n", imm8);
  }
  this->imm[1] = imm8 << 4;
  this->imm[2] = imm8 >> 4;

  if (imm1 > IMM1_MAX) {
    throw new InstructionException("Failed to set imm1, greater than 1 [%d]\n", imm1);
  }
  this->imm[1] |= imm1 << 3;

  if (imm10.size() != IMM10_SIZE || getBytesToULong(imm10) > IMM10_MAX) {
    throw new InstructionException("Failed to set imm10, greater than 1023 [%d]\n", getBytesToULong(imm10));
  }
  this->imm[0] = imm10[0] << 1;
  this->imm[1] |= (imm10[0] & 128) >> 7;
  this->imm[1] |= imm10[1] << 1;

  if (imm31 > IMM1_MAX) {
    throw new InstructionException("Failed to set imm10, greater than 1 [%d]\n", imm31);
  }
  this->imm[2] |= imm31 << 4;
  
  this->type = InstructionType::J;
}

void JTypeInstruction::decode(bytes instruction) {
  if (instruction.size() != INSTRUCTION_SIZE) {
    throw new InstructionException("Failed to decode instruction, not 4 bytes in length [%d]\n", instruction.size());
  }

  try {
    this->opcode = getContrainedBits(instruction, 0, 6)[0];
    this->rd = getContrainedBits(instruction, 7, 11)[0];
    bytes fullImm = getContrainedBits(instruction, 12, 31);
    this->imm[0] = (fullImm[1] & 254);
    this->imm[1] = fullImm[2] & 7;
    this->imm[1] |= (fullImm[1] & 1) << 3;
    this->imm[1] |= fullImm[0] << 4;
    this->imm[2] = fullImm[0] >> 4;
    this->imm[2] |= (fullImm[2] & 8) << 1;
  } catch (exception e) {
    throw (e);
  }

  this->type = InstructionType::J;
}

bytes JTypeInstruction::getImm(ushort low, ushort high) {
  if (low == 12 && high == 19) {
    return getContrainedBits(this->imm, 12, 19);
  } else if (low == 20 && high == 20) {
    bytes imm = bytes(1);
    imm[0] = (this->imm[1] >> 3) & 1;
    return imm;
  } else if (low == 21 && high == 30) {
    return getContrainedBits(this->imm, 1, 10);
  } else if (low == 31 && high == 31) {
    bytes imm = bytes(1);
    imm[0] = this->imm[2] >> 4;
    return imm;
  }

  throw new InstructionException("Failed to get imm, does not exist in this instruction type [low: %d, high: %d]\n", low, high);
}
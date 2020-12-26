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
    throw new InstructionException("Failed to set opcode, greater than 63 [%d]", opcode);
  }
  this->opcode = opcode;

  if (rd > R_MAX) {
    throw new InstructionException("Failed to set rd, greater than 31 [%d]", rd);
  }
  this->rd = rd;

  if (imm8 > IMM8_MAX) {
    throw new InstructionException("Failed to set imm8, greater than 255 [%d]", imm8);
  }
  this->imm[0] = imm8;

  if (imm1 > IMM1_MAX) {
    throw new InstructionException("Failed to set imm1, greater than 1 [%d]", imm1);
  }
  this->imm[1] = imm1;

  ulong imm10Size = getBytesToULong(imm10);
  if (imm10.size() != IMM10_SIZE || imm10Size > IMM10_MAX) {
    throw new InstructionException("Failed to set imm10, greater than 1023 [%d]", imm10Size);
  }
  this->imm[1] |= imm10[0] << 1;
  this->imm[2] = imm10[0] >> 7;
  this->imm[2] |= imm10[1] << 1;

  if (imm31 > IMM1_MAX) {
    throw new InstructionException("Failed to set imm10, greater than 1 [%d]", imm31);
  }
  this->imm[2] |= imm31 << 3;
  
  this->type = InstructionType::J;
}

void JTypeInstruction::decode(bytes instruction) {
  if (instruction.size() != INSTRUCTION_SIZE) {
    throw new InstructionException("Failed to decode instruction, not 4 bytes in length [%d]", instruction.size());
  }

  try {
    this->opcode = getContrainedBits(instruction, 0, 6)[0];
    this->rd = getContrainedBits(instruction, 7, 11)[0];
    this->imm = getContrainedBits(instruction, 12, 31);
  } catch (exception e) {
    throw (e);
  }

  this->type = InstructionType::J;
}

bytes JTypeInstruction::getImm(ushort low, ushort high) {
  if (low == 12 && high == 19) {
    return bytes{this->imm[0]};
  } else if (low == 20 && high == 20) {
    bytes imm = bytes(1);
    imm[0] = this->imm[1] & 1;
    return imm;
  } else if (low == 21 && high == 30) {
    bytes imm = bytes(2);
    imm[0] = this->imm[1] >> 1;
    imm[0] |= this->imm[2] << 7;
    imm[1] = (this->imm[2] >> 1) & 3;
    return imm;
  } else if (low == 31 && high == 31) {
    bytes imm = bytes(1);
    imm[0] = this->imm[2] >> 3;
    return imm;
  }

  throw new InstructionException("Failed to get imm, does not exist in this instruction type [low: %d, high: %d]", low, high);
}
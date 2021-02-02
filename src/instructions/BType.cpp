#include "../include/instructions/BType.h"
#include "../include/bytemanip.h"

BTypeInstruction::BTypeInstruction(ushort XLEN): AbstractInstruction(XLEN) {
  this->opcode = 0;
  this->func3 = 0;
  this->rs1 = 0;
  this->rs2 = 0;
  this->imm = bytes(2);
  this->type = InstructionType::B;
}

BTypeInstruction::BTypeInstruction(ushort XLEN, byte opcode, byte imm1, byte imm4, byte func3, byte rs1, byte rs2, byte imm6, byte imm31): AbstractInstruction(XLEN) {
  this->imm = bytes(2);

  if (opcode > OPCODE_MAX) {
    throw new InstructionException("Failed to set opcode, greater than 63 [%d]\n", opcode);
  }
  this->opcode = opcode;

  if (imm1 > IMM1_MAX) {
    throw new InstructionException("Failed to set imm1, greater than 1 [%d]\n", imm1);
  }
  this->imm[1] = imm1 << 3;

  if (imm4 > IMM4_MAX) {
    throw new InstructionException("Failed to set imm4, greater than 15 [%d]\n", imm4);
  }
  this->imm[0] = imm4 << 1;

  if (func3 > FUNC3_MAX) {
    throw new InstructionException("Failed to set func3, greater than 7 [%d]\n", func3);
  }
  this->func3 = func3;

  if (rs1 > R_MAX) {
    throw new InstructionException("Failed to set rs1, greater than 31 [%d]\n", rs1);
  }
  this->rs1 = rs1;

  if (rs2 > R_MAX) {
    throw new InstructionException("Failed to set rs2, greater than 31 [%d]\n", rs2);
  }
  this->rs2 = rs2;

  if (imm6 > IMM6_MAX) {
    throw new InstructionException("Failed to set imm6, greater than 63 [%d]\n", imm6);
  }
  this->imm[0] |= imm6 << 5;
  this->imm[1] |= imm6 >> 3;

  if (imm31 > IMM1_MAX) {
    throw new InstructionException("Failed to set imm31, greater than 1 [%d]\n", imm31);
  }
  this->imm[1] |= imm31 << 4;

  // Sign extend
  if (imm31) {
    this->imm[1] |= ~getByteMaskForPosition(5);
  }

  this->isSigned = (bool)imm31;

  this->type = InstructionType::B;
}

void BTypeInstruction::decode(bytes instruction) {
  if (instruction.size() != INSTRUCTION_SIZE) {
    throw new InstructionException("Failed to decode instruction, not 4 bytes in length [%d]\n", instruction.size());
  }

  try {
    this->opcode = getContrainedBits(instruction, 0, 6)[0];
    // Imm bit 11 is at position 7 in instruction
    byte imm11 = instruction[0] & 128;
    // Imm bits 1-4 are at positions 8-11 in instruction, needs to be shifted by 1
    this->imm[0] = getContrainedBits(instruction, 8, 11)[0] << 1;
    this->func3 = getContrainedBits(instruction, 12, 14)[0];
    this->rs1 = getContrainedBits(instruction, 15, 19)[0];
    this->rs2 = getContrainedBits(instruction, 20, 24)[0];
    byte imm510 = getContrainedBits(instruction, 25, 30)[0];
    byte imm12 = instruction[3] & 128;
    this->isSigned = (bool)imm12;
    this->imm[0] |= imm510 << 5;
    this->imm[1] = imm510 >> 3; 
    this->imm[1] |= imm11 >> 4;
    this->imm[1] |= imm12 >> 3;
    
    // Sign extend to high
    if (imm12) {
      this->imm[1] |= ~getByteMaskForPosition(5);
    }
  } catch (exception e) {
    throw (e);
  }

  this->type = InstructionType::B;
}

bytes BTypeInstruction::getImm(ushort low, ushort high) {
  if (low == 7 && high == 7) {
    bytes imm = bytes(1);
    imm[0] = (this->imm[1] & 8) >> 3;
    return imm;
  } else if (low == 8 && high == 11) {
    return getContrainedBits(this->imm, 1, 4);
  } else if (low == 25 && high == 30) {
    bytes imm = bytes(1);
    imm[0] = this->imm[0] >> 5;
    imm[0] |= this->imm[1] << 3;
    imm[0] &= 63;
    return imm;
  } else if (low == 31 && high == 31) {
    bytes imm = bytes(1);
    imm[0] = (this->imm[1] >> 4) & 1;
    return imm;
  }

  throw new InstructionException("Failed to get imm, does not exist in this instruction type [low: %d, high: %d]\n", low, high);
}

string BTypeInstruction::debug() {
  return string("");
}
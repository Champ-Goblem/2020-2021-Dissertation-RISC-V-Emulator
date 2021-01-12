#include "../include/instructions/UType.h"

UTypeInstruction::UTypeInstruction() {
  this->opcode = 0;
  this->rd = 0;
  this->imm = bytes(3);
  this->type = InstructionType::U;
}

UTypeInstruction::UTypeInstruction(byte opcode, byte rd, bytes imm) {
  if (opcode > OPCODE_MAX) {
    throw new InstructionException("Failed to set opcode, greater than 63 [%d]\n", opcode);
  }
  this->opcode = opcode;

  if (rd > R_MAX) {
    throw new InstructionException("Failed to set rd, greater than 31 [%d]\n", rd);
  }
  this->rd = rd;

  ulong size = getBytesToULong(imm);
  if (imm.size() != IMM_SIZE || size > IMM_MAX) {
    throw new InstructionException("Failed to set rd, greater than 1048575 [%d]\n", size);
  }
  imm.resize(4);
  this->imm = bytesLogicalLeftShift(imm, 12);
  this->isSigned = (bool)(imm[2] & 8);
  if (this->isSigned) {
    this->imm[1] |= ~getByteMaskForPosition(4);
  }

  this->type = InstructionType::U;  
}

void UTypeInstruction::decode(bytes instruction) {
  if (instruction.size() != INSTRUCTION_SIZE) {
    throw new InstructionException("Failed to decode instruction, not 4 bytes in length [%d]\n", instruction.size());
  }

  try {
    this->opcode = getContrainedBits(instruction, 0, 6)[0];
    this->rd = getContrainedBits(instruction, 7, 11)[0];
    bytes imm = getContrainedBits(instruction, 12, 31);
    imm.resize(4);
    this->imm = bytesLogicalLeftShift(imm, 12);
    this->isSigned = (bool)(imm[2] & 8);
  } catch (exception e) {
    throw (e);
  }

  this->type = InstructionType::U;
}

bytes UTypeInstruction::getImm(ushort low, ushort high) {
  if (low == 12 && high == 31) {
    return getContrainedBits(this->imm, 12, 31);
  }

  throw new InstructionException("Failed to get imm, does not exist in this instruction type [low: %d, high: %d]\n", low, high);
}

string UTypeInstruction::debug() {
  return string("");
}
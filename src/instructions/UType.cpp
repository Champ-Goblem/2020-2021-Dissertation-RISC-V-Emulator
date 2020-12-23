#include "../include/instructions/UType.h"

UTypeInstruction::UTypeInstruction() {
  this->opcode = 0;
  this->rd = 0;
  this->imm = bytes(3);
  this->type = InstructionType::U;
}

UTypeInstruction::UTypeInstruction(byte opcode, byte rd, bytes imm) {
  if (opcode > OPCODE_MAX) {
    throw new AbstractInstructionException("Failed to set opcode, greater than 63 [%d]", opcode);
  }
  this->opcode = opcode;

  if (rd > R_MAX) {
    throw new AbstractInstructionException("Failed to set rd, greater than 31 [%d]", rd);
  }
  this->rd = rd;

  ulong size = getBytesToULong(imm);
  if (imm.size() != IMM_SIZE || size > IMM_MAX) {
    throw new AbstractInstructionException("Failed to set rd, greater than 1048575 [%d]", size);
  }
  this->imm = imm;

  this->type = InstructionType::U;
}

void UTypeInstruction::decode(bytes instruction) {
  if (instruction.size() != INSTRUCTION_SIZE) {
    throw new AbstractInstructionException("Failed to decode instruction, not 4 bytes in length [%d]", instruction.size());
  }

  try {
    this->opcode = getContrainedBits(instruction, 0, 6)[0];
    this->rd = getContrainedBits(instruction, 7, 11)[0];
    this->imm = getContrainedBits(instruction, 12, 31);
  } catch (exception e) {
    throw (e);
  }

  this->type = InstructionType::U;
}

bytes UTypeInstruction::getImm(ushort low, ushort high) {
  if (low == 12 && high == 31) {
    return imm;
  }

  throw new AbstractInstructionException("Failed to get imm, does not exist in this instruction type [low: %d, high: %d]", low, high);
}
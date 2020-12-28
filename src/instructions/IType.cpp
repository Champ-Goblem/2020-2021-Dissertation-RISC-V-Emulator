#include "../include/instructions/IType.h"

ITypeInstruction::ITypeInstruction() {
  this->opcode = 0;
  this->rd = 0;
  this->func3 = 0;
  this->rs1 = 0;
  this->imm = bytes(2);
  this->type = InstructionType::I;
}

ITypeInstruction::ITypeInstruction(byte opcode, byte rd, byte func3, byte rs1, bytes imm) {
  if (imm.size() != 2) {
    throw new InstructionException("Failed to set imm, length not 2 bytes [%d]\n", imm.size());
  }

  ulong valOfImm = getBytesToULong(imm);
  if (valOfImm > MAX_IMM_SIZE) {
    throw new InstructionException("Failed to set imm, size greater than allowed [max: %d, acc: %d]\n", MAX_IMM_SIZE, valOfImm);
  }

  if (opcode > OPCODE_MAX) {
    throw new InstructionException("Failed to set opcode, greater than 63 [%d]\n", opcode);
  }
  this->opcode = opcode;

  if (rd > R_MAX) {
    throw new InstructionException("Failed to set rd, greater than 31 [%d]\n", rd);
  }
  this->rd = rd;

  if (func3 > FUNC3_MAX) {
    throw new InstructionException("Failed to set func3, greater than 7 [%d]\n", func3);
  }
  this->func3 = func3;

  if (rs1 > R_MAX) {
    throw new InstructionException("Failed to set rs1, greater than 31 [%d]\n", rs1);
  }
  this->rs1 = rs1;
  this->imm = imm;

  this->type = InstructionType::I;
}

void ITypeInstruction::decode(bytes instruction) {
  if (instruction.size() != INSTRUCTION_SIZE) {
    throw new InstructionException("Failed to decode instruction, not 4 bytes");
  }

  try {
    this->opcode = getContrainedBits(instruction, 0, 6)[0];
    this->rd = getContrainedBits(instruction, 7, 11)[0];
    this->func3 = getContrainedBits(instruction, 12, 14)[0];
    this->rs1 = getContrainedBits(instruction, 15, 19)[0];
    this->imm = getContrainedBits(instruction, 20, 31);
  } catch (exception e) {
    throw e;
  }

  this->type = InstructionType::I;
}

bytes ITypeInstruction::getImm(ushort low, ushort high) {
  if (low == 20 && high == 31) {
    return this->imm; 
  }
  throw new InstructionException("Failed to get imm, wrong position [low: %d, high: %d]\n", low, high);
}
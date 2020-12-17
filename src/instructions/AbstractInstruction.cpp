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
bytes AbstractInstruction::getImm(ushort low, ushort high) {
  if (low >= 32 || high >= 32) {
    throw new AbstractInstructionException("Failed to get immediate, access to out of bounds index [lo: %d, hi: %d, max: 32]", low, high);
  }
  if (low > high) {
    throw new AbstractInstructionException("Failed to get immediate, low bigger than high [lo: %d, hi: %d]", low, high);
  }

  ushort noBytes = ceil((high - low) / 8);
  // New imm
  bytes constrainedImm = bytes (noBytes);
  // Starting byte of old imm
  ushort start = ceil(low / 8);
  // Calculate how many bits the low is inside of a byte
  ushort offset = low - (8 * start);

  for (int i=0; i < noBytes; i++) {
    // Get the initial bits from the old byte
    byte tmp = imm[start + i] << offset;

    // Four cases for when we shift the bytes down
    // 1. high > offset into next byte, copy the offset from next byte into this one
    if (high >= (((start + 1) * 8) + offset)) {
      byte hiTmp = imm[start + i + 1] >> (8 - offset);
      tmp |= hiTmp;
    // 2. high < offset into next byte but high > end of this byte, copy the remaining bits into current byte
    } else if (high < (((start + 1) * 8) + offset) && high > ((start + 1) * 8)) {
      ushort c = ((start + 1) * 8) - high;
      // lshift the bits we want to high
      byte hiTmp = imm[start + i + 1] >> c;
      hiTmp <<= offset - c;
      tmp |= hiTmp;
    // 3. high < end of this byte, only take the high - low number of bits
    } else if (high < ((start + 1) * 8)) {
      ushort c = high - (start * 8);
      tmp &= getByteMaskForPosition(c + offset);
    }
    // 4. high = end of this byte, do nothing
    constrainedImm[i] = tmp;
  }

  return constrainedImm;
};
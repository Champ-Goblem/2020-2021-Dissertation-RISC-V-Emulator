#ifndef __BYTEMANIP__
#define __BYTEMANIP__

#include "emustd.h"
#include <exception>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

static string getBytesForPrint(bytes data) {
  ushort size = data.size();
  ostringstream str;
  for (int i=0; i < size; i++) {
    str << setw(2) << setfill('0') << hex << (int) data[size - 1 - i] << " ";
  }
  str << "\n";
  return str.str();
}


// Returns a byte mask with all 1s up to position defined by position in the byte
static byte getByteMaskForPosition(ushort position) {
  if (position >= 8) {
    return 255;
  }
  byte out = 0;
  for (int i=0; i<=position; i++) {
    out += pow(2, i);
  }
  return out;
}

static bytes getContrainedBits(bytes imm, ushort low, ushort high) {
  // assumes low is 0 and high is 31
  if (low >= imm.size() * 8 || high >= imm.size() * 8) {
    throw new EmulatorException("Failed to get immediate, access to out of bounds index [lo: %d, hi: %d, max: %d]", low, high, imm.size() * 8);
  }
  if (low > high) {
    throw new EmulatorException("Failed to get immediate, low bigger than high [lo: %d, hi: %d]", low, high);
  }

  ushort noBytes = ceil((float)(high - low) / 8);
  // New imm
  bytes constrainedImm = bytes (noBytes);
  // Starting byte of old imm
  ushort start = floor((float)low / 8);
  // Calculate how many bits the low is inside of a byte
  ushort offset = low - (8 * start);

  for (int i=0; i < noBytes; i++) {
    // Get the initial bits from the old imm
    byte tmp = imm[start + i] >> offset;

    // Four cases for when we shift the bytes down
    // 1. high > offset into next byte, copy the offset from next byte into this one
    if (high >= (((start + 1 + i) * 8) + offset)) {
      byte hiTmp = imm[start + i + 1] << (8 - offset);
      tmp |= hiTmp;
    // 2. high < offset into next byte but high > end of this byte, copy the remaining bits into current byte
    } else if (high < (((start + 1 + i) * 8) + offset) && high >= ((start + i + 1) * 8)) {
      ushort c = ((start + 2 + i) * 8) - high - 1;
      // lshift the bits we want to high
      byte hiTmp = imm[start + i + 1] << c;
      byte newHiTmp = hiTmp >> abs(offset - c);
      tmp |= newHiTmp;
    // 3. high < end of this byte, only take the high - low number of bits
    } else if (high < ((start + 1 + i) * 8)) {
      ushort c = high - ((start + i) * 8);
      ushort a = getByteMaskForPosition(c - offset);
      tmp &= a;
    }
    // 4. high = end of this byte, do nothing
    constrainedImm[i] = tmp;
  }

  return constrainedImm;
}

static ulong getBytesToULong(bytes val) {
  if (val.size() > 8) {
    throw new EmulatorException("Number of bytes of value is greater than ulong, possible overflow [%d]", val.size());
  }
  ulong result = 0;
  for (int i=0; i < val.size(); i++) {
    result += val[i] * pow(2, 8 * i);
  }

  return result;
}

static bytes addByteToBytes(bytes val, byte operand) {
  if (val.size() == 0) {
    throw new EmulatorException("Failed to add a byte to bytes, size of bytes is zero");
  }

  bytes ret = bytes(val);
  byte firstByte = val[0];
  ret[0] = firstByte + operand;
  ushort remainder = ((ushort)firstByte + operand) % 255;
  ushort currentByte = 1;
  while(remainder > 0 && currentByte < val.size()) {
    byte tmp = val[currentByte];
    ret[currentByte] = tmp + remainder;
    remainder = ((ushort)tmp + remainder) % 255;
    currentByte++;
  }

  if (remainder != 0) {
    throw new EmulatorException("Overflow when adding a byte to bytes");
  }

  return ret;
}

static bytes addBytesToBytes(bytes val, bytes operand) {
  if (val.size() == 0 || operand.size() == 0) {
    throw new EmulatorException("Failed to add bytes to bytes, size of operand is zero");
  }

  bytes ret = bytes(val);
  byte remainder = 0;
  for (uint i=0; i<operand.size(); i++) {
    ret[i] = val[i] + operand[i] + remainder;
    remainder = ((ushort)val[i] + operand[i] + remainder) % 255;
  }

  if (remainder != 0) {
    throw new EmulatorException("Overflow when adding bytes to bytes");
  }

  return ret;
}

static bytes subBytesFromBytes(bytes val, bytes operand) {
  if (val.size() == 0 || operand.size() == 0) {
    throw new EmulatorException("Failed to subtract bytes from bytes, size of operand is zero");
  }

  bytes ret = bytes(val);
  byte remainder = 0;
  for (uint i=0; i<operand.size(); i++) {
    ret[i] = val[i] - operand[i] - remainder;
    remainder = ((ushort)val[i] - operand[i] - remainder) % 255;
  }

  if (remainder != 0) {
    throw new EmulatorException("Underflow when subtracting bytes from bytes");
  }

  return ret;
}

#endif
#ifndef __BYTEMANIP__
#define __BYTEMANIP__

#include "emustd.h"
#include <exception>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cmath>

static string getBytesForPrint(bytes data) {
  ushort size = data.size();
  ostringstream str;
  for (int i=0; i < size; i++) {
    str << setw(2) << setfill('0') << hex << (int) data[size - 1 - i] << " ";
  }
  return str.str();
}


// Returns a byte mask with all 1s up to position defined by position in the byte
static byte getByteMaskForPosition(ushort position) {
  if (position >= 8) {
    return 255;
  }
  byte out = 0;
  for (int i=1; i<=position; i++) {
    out += pow(2, i-1);
  }
  return out;
}

static bytes getContrainedBits(bytes imm, ushort low, ushort high) {
  // assumes low is 0 and high is 31
  if (low >= imm.size() * 8 || high >= imm.size() * 8) {
    throw new EmulatorException("Failed to get immediate, access to out of bounds index");
  }
  if (low > high) {
    throw new EmulatorException("Failed to get immediate, low bigger than high");
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
      ushort a = getByteMaskForPosition(c - offset + 1);
      tmp &= a;
    }
    // 4. high = end of this byte, do nothing
    constrainedImm[i] = tmp;
  }

  return constrainedImm;
}

static ulong getBytesToULong(bytes val) {
  if (val.size() > 8) {
    throw new EmulatorException("Number of bytes of value is greater than ulong, possible overflow");
  }
  ulong result = 0;
  for (int i=0; i < val.size(); i++) {
    result += val[i] * pow(2, 8 * i);
  }

  return result;
}

static bytes copyWithSignExtend(bytes val, uint size) {
  if (size < val.size()) {
    throw EmulatorException("Failed to copy bytes with sign extend, target size smaller than current size\n");
  }

  if (size == val.size()) {
    return val;
  }

  bytes result(size);
  bool isSigned = (bool)(val[val.size() - 1] & 128);
  for (uint i=0; i<size; i++) {
    if (i < val.size()) {
      result[i] = val[i];
    } else if (isSigned) {
      result[i] = 255;
    }
  }

  return result;
}

static bytes addByteToBytes(bytes val, byte operand) {
  if (val.size() == 0) {
    throw new EmulatorException("Failed to add a byte to bytes, size of bytes is zero\n");
  }

  bytes ret = bytes(val);
  byte firstByte = val[0];
  ret[0] = firstByte + operand;
  ushort remainder = ((ushort)firstByte + operand) / 255;
  ushort currentByte = 1;
  while(remainder > 0 && currentByte < val.size()) {
    byte tmp = val[currentByte];
    ret[currentByte] = tmp + remainder;
    remainder = ((ushort)tmp + remainder) / 255;
    currentByte++;
  }

  if (remainder != 0) {
    throw new EmulatorException("Overflow when adding a byte to bytes\n");
  }

  return ret;
}

// static bytes addBytesToBytes(bytes val, bytes operand) {
//   if (val.size() == 0 || operand.size() == 0) {
//     throw new EmulatorException("Failed to add bytes to bytes, size of operand is zero\n");
//   }

//   bytes ret = bytes(val);
//   byte remainder = 0;
//   for (uint i=0; i<operand.size(); i++) {
//     ret[i] = val[i] + operand[i] + remainder;
//     remainder = ((ushort)val[i] + operand[i] + remainder) / 255;
//   }

//   if (remainder != 0) {
//     throw new EmulatorException("Overflow when adding bytes to bytes\n");
//   }

//   return ret;
// }

// static bytes subBytesFromBytes(bytes val, bytes operand) {
//   if (val.size() == 0 || operand.size() == 0) {
//     throw new EmulatorException("Failed to subtract bytes from bytes, size of operand is zero\n");
//   }

//   bytes ret = bytes(val);
//   byte remainder = 0;
//   for (uint i=0; i<operand.size(); i++) {
//     ret[i] = val[i] - operand[i] - remainder;
//     short abs = (short)val[i] - operand[i] - remainder; 
//     if (abs < 0) {
//       remainder = ((ushort)std::abs(abs) / 255) + 1;
//     } else {
//       remainder = 0;
//     }
//   }

//   if (remainder != 0) {
//     throw new EmulatorException("Underflow when subtracting bytes from bytes\n");
//   }

//   return ret;
// }

static bytes bytesAddition(bytes val1, bytes val2, bool throwForFlows=false) {
  uint size1 = val1.size();
  uint size2 = val2.size();
  if (size1 == 0 || size2 == 0) {
    throw EmulatorException("Failed to add bytes signed, size of operand is zero\n");
  }

  // Make sure both values are the same length and thus have also been sign extended
  bytes workingVal1 = val1;
  bytes workingVal2 = val2;
  if (size1 < size2) {
    workingVal1 = copyWithSignExtend(val1, size2);
  } else if (size2 < size1) {
    workingVal2 = copyWithSignExtend(val2, size1);
  }

  bytes result{workingVal1};
  byte remainder = 0;
  uint i = 0;
  while (i < size1) {
    result[i] = workingVal1[i] + workingVal2[i] + remainder;
    remainder = ((ushort)workingVal1[i] + workingVal2[i] + remainder) / 255;
    i++;
  }

  if (throwForFlows && (remainder || i < size2)) {
    throw EmulatorException("Overflow when adding two numbers");
  }

  return result;
}

static bool bytesLessThanBytesSigned(bytes val1, bytes val2) {
  uint size1 = val1.size();
  uint size2 = val2.size();
  if (size1 == 0 || size2 == 2) {
    throw EmulatorException("Failed to check less than signed, size of operand is zero\n");
  }

  uint count1 = val1.size() - 1;
  uint count2 = val2.size() - 1;
  while (count1 >= 0 && count2 >= 0) {
    char v1 = count1 >= count2 ? (char)val1[count1] : 0;
    char v2 = count2 >= count1 ? (char)val2[count2] : 0;
    if (v1 < v2) {
      return true;
    } else if (v1 > v2) {
      return false;
    }
  }

  count1 = count1 - (count1 >= count2 ? 1 : 0);
  count2 = count2 - (count2 >= count1 ? 1 : 0);

  return false;
}

static bool bytesLessThanBytesUnsigned(bytes val1, bytes val2) {
  uint size1 = val1.size();
  uint size2 = val2.size();
  if (size1 == 0 || size2 == 2) {
    throw EmulatorException("Failed to check less than unsigned, size of operand is zero\n");
  }

  uint count1 = val1.size() - 1;
  uint count2 = val2.size() - 1;
  while (count1 >= 0 && count2 >= 0) {
    char v1 = count1 >= count2 ? val1[count1] : 0;
    char v2 = count2 >= count1 ? val2[count2] : 0;
    if (v1 < v2) {
      return true;
    } else if (v1 > v2) {
      return false;
    }
  }

  count1 = count1 - (count1 >= count2 ? 1 : 0);
  count2 = count2 - (count2 >= count1 ? 1 : 0);

  return false;
}

static bool bytesGreaterOrequalToSigned(bytes val1, bytes val2) {
  uint size1 = val1.size();
  uint size2 = val2.size();
  if (size1 == 0 || size2 == 2) {
    throw EmulatorException("Failed to check ge signed, size of operand is zero\n");
  }

  uint count1 = val1.size() - 1;
  uint count2 = val2.size() - 1;
  while (count1 >= 0 && count2 >= 0) {
    char v1 = count1 >= count2 ? (char)val1[count1] : 0;
    char v2 = count2 >= count1 ? (char)val2[count2] : 0;
    if (v1 > v2) {
      return true;
    } else if (v1 < v2) {
      return false;
    }
  }

  count1 = count1 - (count1 >= count2 ? 1 : 0);
  count2 = count2 - (count2 >= count1 ? 1 : 0);

  return true;
}

static bool bytesGreaterOrequalToUnsigned(bytes val1, bytes val2) {
  uint size1 = val1.size();
  uint size2 = val2.size();
  if (size1 == 0 || size2 == 2) {
    throw EmulatorException("Failed to check ge unsigned, size of operand is zero\n");
  }

  uint count1 = val1.size() - 1;
  uint count2 = val2.size() - 1;
  while (count1 >= 0 && count2 >= 0) {
    char v1 = count1 >= count2 ? val1[count1] : 0;
    char v2 = count2 >= count1 ? val2[count2] : 0;
    if (v1 > v2) {
      return true;
    } else if (v1 < v2) {
      return false;
    }
  }

  count1 = count1 - (count1 >= count2 ? 1 : 0);
  count2 = count2 - (count2 >= count1 ? 1 : 0);

  return true;
}

static bytes bytesLogicalLeftShift(bytes val, uint shift) {
  // Check that the value we are working on is not 0 bytes long
  uint size1 = val.size();
  if (size1 == 0) {
    throw EmulatorException("Failed to left shift, size of operand is zero\n");
  }

  // Calculate the number of full shifts that will be done (i.e byte = 0 after << 8)
  uint fullShift = shift / 8;
  // Calculate offset to shift each byte by, will be zero if the shift is a multiple of 8 (i.e shifting by a full byte)
  uint offset = shift - (fullShift * 8);
  // Create new array with enough space for the current byte + full shift, any overflow is ignored
  bytes result = bytes(size1 + fullShift);
  byte carry = 0;
  for (uint i=0; i<size1; i++) {
    // By starting from i+fullShift these "full shifts" will all be zero in the result
    result[i+fullShift] = val[i] << offset;
    // Copy any remaining carries from last operation
    result[i+fullShift] |= carry;
    // Calculate new carry
    carry = val[i] >> (8-offset);
  }

  return result;
}

#endif
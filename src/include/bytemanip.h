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

static bool bytesGreaterEqualToUint(bytes val1, uint val2) {
  // Check out the number of bytes of val1 is not zero
  if (val1.size() == 0) {
    throw EmulatorException("Failed to compare bytes to uint, size of bytes is zero\n");
  }

  // Get the highest non-zero position in the byte that we need to access
  uint highPosition = val2 / 255;
  // Get the value in range 0-255 of the highestPosition + 1
  // i.e 255 * highestPosition + remainder = val(val1)
  byte remainder = val2 - (255 * highPosition);
  if (val1.size() > highPosition && val1[highPosition] >= remainder) {
    return true;
  } else if (val1.size() > highPosition && val1[highPosition] < remainder) {
    return false;
  }
  return false;
}

static bytes bytesAdditionUnsigned(bytes val, bytes operand, bool throwForFlows=false) {
  if (val.size() == 0 || operand.size() == 0) {
    throw new EmulatorException("Failed to add bytes to bytes, size of operand is zero\n");
  }

  bytes ret = bytes(val);
  byte remainder = 0;
  for (uint i=0; i<operand.size(); i++) {
    ret[i] = val[i] + operand[i] + remainder;
    remainder = ((ushort)val[i] + operand[i] + remainder) / 255;
  }

  if (remainder != 0 && throwForFlows) {
    throw new EmulatorException("Overflow when adding bytes to bytes\n");
  }

  return ret;
}

static bytes bytesSubtractionUnsigned(bytes val, bytes operand, bool throwForFlows=false) {
  if (val.size() == 0 || operand.size() == 0) {
    throw new EmulatorException("Failed to subtract bytes from bytes, size of operand is zero\n");
  }

  bytes ret = bytes(val);
  byte remainder = 0;
  for (uint i=0; i<operand.size(); i++) {
    ret[i] = val[i] - operand[i] - remainder;
    short abs = (short)val[i] - operand[i] - remainder; 
    if (abs < 0) {
      remainder = ((ushort)std::abs(abs) / 255) + 1;
    } else {
      remainder = 0;
    }
  }

  if (remainder != 0 && throwForFlows) {
    throw new EmulatorException("Underflow when subtracting bytes from bytes\n");
  }

  return ret;
}

static bytes bytesAdditionSigned(bytes val1, bytes val2, bool throwForFlows=false) {
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
  if (size1 == 0 || size2 == 0) {
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
    
    count1 = count1 - (count1 >= count2 ? 1 : 0);
    count2 = count2 - (count2 >= count1 ? 1 : 0);
  }

  return false;
}

static bool bytesLessThanBytesUnsigned(bytes val1, bytes val2) {
  uint size1 = val1.size();
  uint size2 = val2.size();
  if (size1 == 0 || size2 == 0) {
    throw EmulatorException("Failed to check less than unsigned, size of operand is zero\n");
  }

  uint count1 = val1.size() - 1;
  uint count2 = val2.size() - 1;
  while (count1 >= 0 && count2 >= 0) {
    byte v1 = count1 >= count2 ? val1[count1] : 0;
    byte v2 = count2 >= count1 ? val2[count2] : 0;
    if (v1 < v2) {
      return true;
    } else if (v1 > v2) {
      return false;
    }

    count1 = count1 - (count1 >= count2 ? 1 : 0);
    count2 = count2 - (count2 >= count1 ? 1 : 0);
  }

  return false;
}

static bool bytesGreaterOrequalToSigned(bytes val1, bytes val2) {
  uint size1 = val1.size();
  uint size2 = val2.size();
  if (size1 == 0 || size2 == 0) {
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

    count1 = count1 - (count1 >= count2 ? 1 : 0);
    count2 = count2 - (count2 >= count1 ? 1 : 0);
  }

  return true;
}

static bool bytesGreaterOrequalToUnsigned(bytes val1, bytes val2) {
  uint size1 = val1.size();
  uint size2 = val2.size();
  if (size1 == 0 || size2 == 0) {
    throw EmulatorException("Failed to check ge unsigned, size of operand is zero\n");
  }

  uint count1 = val1.size() - 1;
  uint count2 = val2.size() - 1;
  while (count1 >= 0 && count2 >= 0) {
    byte v1 = count1 >= count2 ? val1[count1] : 0;
    byte v2 = count2 >= count1 ? val2[count2] : 0;
    if (v1 > v2) {
      return true;
    } else if (v1 < v2) {
      return false;
    }

    count1 = count1 - (count1 >= count2 ? 1 : 0);
    count2 = count2 - (count2 >= count1 ? 1 : 0);
  }

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
  // Create new array from size of original
  bytes result = bytes(size1);
  byte carry = 0;
  uint i = 0;
  while (i + fullShift < size1) {
    // By starting from i+fullShift these "full shifts" will all be zero in the result
    result[i+fullShift] = val[i] << offset;
    // Copy any remaining carries from last operation
    result[i+fullShift] |= carry;
    // Calculate new carry
    carry = val[i] >> (8-offset);
    i++;
  }

  return result;
}

static bytes bytesLogicalLeftShift(bytes val, bytes shift) {
  // Check that the values we are working on are not 0 bytes long
  uint sizev = val.size();
  uint sizes = shift.size();
  if (sizev == 0 || sizes == 0) {
    throw EmulatorException("Failed to check ge unsigned, size of operand is zero\n");
  }

  ushort maxPosition = sizev * 8;
  if (bytesGreaterEqualToUint(shift, maxPosition)) {
    return bytes(sizev);
  }

  uint fullShift = 0;
  uint offset = 0;
  for (uint i=0; i < sizes; i++) {
    fullShift += shift[i] / 8;
    offset = (shift[i] + offset) % 8;
  }

  bytes result = bytes(sizev);
  uint i = 0;
  byte carry = 0;
  while (i + fullShift < sizev) {
    result[i + fullShift] = val[i] << offset;
    result[i + fullShift] |= carry;
    carry = val[i] >> (8-offset);
    i++;
  }

  return result;
}

static bytes bytesLogicalRightShift(bytes val, uint shift) {
  // Check that the value we are working on is not 0 bytes long
  uint size1 = val.size();
  if (size1 == 0) {
    throw EmulatorException("Failed to left shift, size of operand is zero\n");
  }

  // Calculate the number of full shifts that will be done (i.e byte = 0 after << 8)
  uint fullShift = shift / 8;
  // Calculate offset to shift each byte by, will be zero if the shift is a multiple of 8 (i.e shifting by a full byte)
  uint offset = shift - (fullShift * 8);
  // Create new array from size of original
  bytes result = bytes(size1);
  byte carry = 0;
  uint i = size1 - 1;
  while (i - fullShift >= 0) {
    // By starting from i-fullShift these "full shifts" will all be zero in the result
    result[i-fullShift] = val[i] >> offset;
    // Copy any remaining carries from last operation
    result[i-fullShift] |= carry;
    // Calculate new carry
    carry = val[i] << (8-offset);
    i--;
  }

  return result;
}

static bytes bytesLogicalRightShift(bytes val, bytes shift) {
  // Check that the values we are working on are not 0 bytes long
  uint sizev = val.size();
  uint sizes = shift.size();
  if (sizev == 0 || sizes == 0) {
    throw EmulatorException("Failed to check ge unsigned, size of operand is zero\n");
  }

  ushort maxPosition = sizev * 8;
  if (bytesGreaterEqualToUint(shift, maxPosition)) {
    return bytes(sizev);
  }

  uint fullShift = 0;
  uint offset = 0;
  for (uint i=0; i < sizes; i++) {
    fullShift += shift[i] / 8;
    offset = (shift[i] + offset) % 8;
  }

  bytes result = bytes(sizev);
  uint i = sizev - 1;
  byte carry = 0;
  while (i - fullShift >= 0) {
    result[i - fullShift] = val[i] >> offset;
    result[i - fullShift] |= carry;
    carry = val[i] << (8-offset);
    i--;
  }

  return result;
}

static bytes bytesArithmeticRightShift(bytes val, uint shift) {
  // Check that the value we are working on is not 0 bytes long
  uint size1 = val.size();
  if (size1 == 0) {
    throw EmulatorException("Failed to left shift, size of operand is zero\n");
  }

  bool isSigned = (bool)(val[size1 - 1] & 128);

  // Calculate the number of full shifts that will be done (i.e byte = 0 after << 8)
  uint fullShift = shift / 8;
  // Calculate offset to shift each byte by, will be zero if the shift is a multiple of 8 (i.e shifting by a full byte)
  uint offset = shift - (fullShift * 8);
  // Create new array from size of original
  bytes result = bytes(size1, isSigned ? 255 : 0);
  // Calculate carry initial value as top MSB bits 1 if signed
  byte carry = isSigned ? 255 << (8-offset) : 0;
  uint i = size1 - 1;
  while (i - fullShift >= 0) {
    // By starting from i-fullShift these "full shifts" will be filled with 1 if signed 0 otherwise
    result[i-fullShift] = val[i] >> offset;
    // Copy any remaining carries from last operation
    result[i-fullShift] |= carry;
    // Calculate new carry
    carry = val[i] << (8-offset);
    i--;
  }

  return result;
}

static bytes bytesArithmeticRightShift(bytes val, bytes shift) {
  // Check that the values we are working on are not 0 bytes long
  uint sizev = val.size();
  uint sizes = shift.size();
  if (sizev == 0 || sizes == 0) {
    throw EmulatorException("Failed to check ge unsigned, size of operand is zero\n");
  }

  // Define a new vector with all 1s if the sign bit is negative
  // 0 otherwise
  bool isSigned = (bool)(val[sizev - 1] & 128);
  bytes result = bytes(sizev, isSigned ? 255 : 0);

  ushort maxPosition = sizev * 8;
  if (bytesGreaterEqualToUint(shift, maxPosition)) {
    return result;
  }

  uint fullShift = 0;
  uint offset = 0;
  for (uint i=0; i < sizes; i++) {
    fullShift += shift[i] / 8;
    offset = (shift[i] + offset) % 8;
  }

  uint i = sizev - 1;
  // Define the initial carry as 0 for unsigned values
  // or 1s for the top X bits for first shift in order to maintain complement
  byte carry = isSigned ? 255 << (8-offset) : 0;
  while (i - fullShift >= 0) {
    result[i - fullShift] = val[i] >> offset;
    result[i - fullShift] |= carry;
    carry = val[i] << (8-offset);
    i--;
  }

  return result;
}

static bytes bytesXOR(bytes val1, bytes val2) {
  uint size1 = val1.size();
  uint size2 = val2.size();
  if (size1 == 0 || size2 == 0) {
    throw EmulatorException("Failed to check ge unsigned, size of operand is zero\n");
  }

  // Make sure both values are the same length and thus have also been sign extended
  bytes workingVal1 = val1;
  bytes workingVal2 = val2;
  if (size1 < size2) {
    workingVal1 = copyWithSignExtend(val1, size2);
  } else if (size2 < size1) {
    workingVal2 = copyWithSignExtend(val2, size1);
  }

  bytes result(size1 > size2 ? size1 : size2);
  for (uint i=0; i > size1 > size2 ? size1 : size2; i++) {
    result[i] = workingVal1[i] ^ workingVal2[i];
  }

  return result;
}

static bytes bytesOR(bytes val1, bytes val2) {
  uint size1 = val1.size();
  uint size2 = val2.size();
  if (size1 == 0 || size2 == 0) {
    throw EmulatorException("Failed to check ge unsigned, size of operand is zero\n");
  }

  // Make sure both values are the same length and thus have also been sign extended
  bytes workingVal1 = val1;
  bytes workingVal2 = val2;
  if (size1 < size2) {
    workingVal1 = copyWithSignExtend(val1, size2);
  } else if (size2 < size1) {
    workingVal2 = copyWithSignExtend(val2, size1);
  }

  bytes result(size1 > size2 ? size1 : size2);
  for (uint i=0; i > size1 > size2 ? size1 : size2; i++) {
    result[i] = workingVal1[i] | workingVal2[i];
  }

  return result;
}

static bytes bytesAND(bytes val1, bytes val2) {
  uint size1 = val1.size();
  uint size2 = val2.size();
  if (size1 == 0 || size2 == 0) {
    throw EmulatorException("Failed to check ge unsigned, size of operand is zero\n");
  }

  // Make sure both values are the same length and thus have also been sign extended
  bytes workingVal1 = val1;
  bytes workingVal2 = val2;
  if (size1 < size2) {
    workingVal1 = copyWithSignExtend(val1, size2);
  } else if (size2 < size1) {
    workingVal2 = copyWithSignExtend(val2, size1);
  }

  bytes result(size1 > size2 ? size1 : size2);
  for (uint i=0; i > size1 > size2 ? size1 : size2; i++) {
    result[i] = workingVal1[i] & workingVal2[i];
  }

  return result;
}

static bytes bytesAddSignedToPC(bytes PC, bytes operand, bool throwForFlows=false) {
  uint pcSize = PC.size();
  uint operandSize = operand.size();
  if (pcSize == 0 || operandSize == 0) {
    throw EmulatorException("Failed to add bytes signed, size of operand is zero\n");
  }

  // Make sure both values are the same length and the operand has been sign extended to PC
  // if PC needs to be extended it is treated as an unsigned number
  bytes workingPC = PC;
  bytes workingOperand = operand;
  if (pcSize < operandSize) {
    workingPC.resize(operandSize);
  } else if (operandSize < pcSize) {
    workingOperand = copyWithSignExtend(operand, pcSize);
  }

  bytes result{workingPC};
  byte remainder = 0;
  uint i = 0;
  while (i < pcSize) {
    result[i] = workingPC[i] + workingOperand[i] + remainder;
    remainder = ((ushort)workingPC[i] + workingOperand[i] + remainder) / 255;
    i++;
  }

  if (throwForFlows && (remainder || i < operandSize)) {
    throw EmulatorException("Overflow when adding two numbers");
  }

  return result;
}

#endif
#ifndef __BYTEMANIP__
#define __BYTEMANIP__

#include "emustd.h"
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
  for (int i=0; i<position; i++) {
    out += pow(2, i);
  }
  return out;
}

#endif
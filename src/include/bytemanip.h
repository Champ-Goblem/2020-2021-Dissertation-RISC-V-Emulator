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

#endif
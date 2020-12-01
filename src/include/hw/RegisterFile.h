#ifndef __RegisterFile__
#define __RegisterFile__

#define REG32 32
#define REG16 16

#include "../emustd.h"
#include <map>

typedef map<ushort, bytes> RegisterMap;
typedef RegisterMap::iterator RegisterIterator;

class RegisterFile {
  private:
  RegisterMap registerMap;
  bytes pc;
  bool RV32E;
  ushort XLEN;

  public:
  RegisterFile(ushort XLEN, bool isRV32E);
  bytes get(ushort reg);
  void write(ushort reg, bytes val);
  void debug();
};

#endif
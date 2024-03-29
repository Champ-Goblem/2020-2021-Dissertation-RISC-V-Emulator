#ifndef __RegisterFile__
#define __RegisterFile__

#include <exception>
#define REG32 32
#define REG16 16

#include "../emustd.h"
#include <map>
#include <mutex>

typedef map<ushort, bytes> RegisterMap;
typedef RegisterMap::iterator RegisterIterator;

class RegisterFileException: public EmulatorException {
  public:
  RegisterFileException(): EmulatorException() {}
  RegisterFileException(const char* message, ...): EmulatorException(message){};
};

class RegisterFile {
  private:
  RegisterMap registerMap;
  bool RV32E;
  ushort XLEN;
  mutable mutex lock;

  public:
  RegisterFile(ushort XLEN, bool isRV32E);
  bytes get(ushort reg);
  void write(ushort reg, bytes val);
  vector<bytes> debug();
};

#endif
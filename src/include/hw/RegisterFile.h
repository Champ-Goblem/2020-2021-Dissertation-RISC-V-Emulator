#ifndef __RegisterFile__
#define __RegisterFile__

#include <exception>
#define REG32 32
#define REG16 16

#include "../emustd.h"
#include <map>

typedef map<ushort, bytes> RegisterMap;
typedef RegisterMap::iterator RegisterIterator;

class RegisterFileException: public exception {
  private:
  const char* message;

  public:
  RegisterFileException() {}
  RegisterFileException(const char* message, ...) {
    this->message = message;
  }
  const char* getMessage() {
    return message;
  }
};

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
  void writePC(bytes addr);
  bytes getPC();
  void debug();
};

#endif
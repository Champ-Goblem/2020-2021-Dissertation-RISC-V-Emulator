#ifndef __Memory__
#define __Memory__

#include "../emustd.h"
#include "../bytemanip.h"

#define WORD_SIZE 2
#define DWORD_SIZE 4
#define QWORD_SIZE 8

class MemoryException: public exception {
  private:
  const char* message;
  ulong size;

  public:
  MemoryException() {}
  MemoryException(const char* message, ...) {
    this->message = message;
  }
  const char* getMessage() {
    return message;
  }
};

class Memory {
  private:
  bytes memory;
  ulong size;

  public:
  Memory(ulong size);
  void writeByte(ulong addr, byte val);
  byte readByte(ulong addr);
  void writeWord(ulong addr, bytes val);
  bytes readWord(ulong addr);
  void writeDWord(ulong addr, bytes val);
  bytes readDWord(ulong addr);
  void writeQWord(ulong addr, bytes val);
  bytes readQWord(ulong addr);
  ulong getSize();

  void printRegion(ulong start, ulong count, ushort outWidth);
};

#endif
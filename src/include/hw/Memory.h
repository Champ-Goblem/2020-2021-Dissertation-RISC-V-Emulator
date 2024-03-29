#ifndef __Memory__
#define __Memory__

#include "../emustd.h"
#include <mutex>

#define HWORD_SIZE 2
#define WORD_SIZE 4
#define DWORD_SIZE 8
#define QWORD_SIZE 16

class MemoryException: public EmulatorException {
  public:
  MemoryException(): EmulatorException() {}
  MemoryException(ostringstream message): EmulatorException(message.str()) {}
};

class Memory {
  private:
  bytes memory;
  ulong size;
  mutable mutex lock;

  public:
  Memory(ulong size);
  void writeByte(ulong addr, byte val);
  byte readByte(ulong addr);
  void writeHWord(ulong addr, bytes val);
  bytes readHWord(ulong addr);
  void writeWord(ulong addr, bytes val);
  bytes readWord(ulong addr);
  void writeDWord(ulong addr, bytes val);
  bytes readDWord(ulong addr);
  void writeQWord(ulong addr, bytes val);
  bytes readQWord(ulong addr);
  ulong getSize();

  vector<byte> getRegion(ulong start, ulong size);
  void printRegion(ulong start, ulong count, ushort outWidth);
};

#endif
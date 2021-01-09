#include "../include/hw/Memory.h"

Memory::Memory(ulong size) {
  memory = bytes(size);
  this->size = size;
}

void Memory::writeByte(ulong addr, byte val) {
  if (addr >= size) {
    throw new AddressOutOfMemoryException(addr, 0, size, false);
  }
  memory[addr] = val;
}

byte Memory::readByte(ulong addr) {
  if (addr >= size) {
    throw new AddressOutOfMemoryException(addr, 0, size, true);
  }
  return memory[addr];
}

void Memory::writeHWord(ulong addr, bytes val) {
  if (addr + HWORD_SIZE >= size) {
    throw new AddressOutOfMemoryException(addr, HWORD_SIZE, size, false);
  }
  if (val.size() != HWORD_SIZE) {
    throw new WrongSizeInstructionException(val.size(), HWORD_SIZE);
  }
  memory[addr] = val[0];
  memory[addr+1] = val[1];
}

bytes Memory::readHWord(ulong addr) {
  if (addr + HWORD_SIZE >= size) {
    throw new AddressOutOfMemoryException(addr, HWORD_SIZE, size, true);
  }
  return bytes {memory[addr], memory[addr+1]};
}

void Memory::writeWord(ulong addr, bytes val) {
  if (addr + WORD_SIZE >= size) {
    throw new AddressOutOfMemoryException(addr, WORD_SIZE, size, false);
  }
  if (val.size() != WORD_SIZE) {
    throw new WrongSizeInstructionException(val.size(), WORD_SIZE);
  }
  memory[addr] = val[0];
  memory[addr+1] = val[1];
  memory[addr+2] = val[2];
  memory[addr+3] = val[3];
}

bytes Memory::readWord(ulong addr) {
  if (addr + WORD_SIZE >= size) {
    throw new AddressOutOfMemoryException(addr, WORD_SIZE, size, true);
  }
  return bytes {memory[addr], memory[addr+1], memory[addr+2], memory[addr+3]};
}

void Memory::writeDWord(ulong addr, bytes val) {
  if (addr + DWORD_SIZE >= size) {
    throw new AddressOutOfMemoryException(addr, DWORD_SIZE, size, false);
  }
  if (val.size() != DWORD_SIZE) {
    throw new WrongSizeInstructionException(val.size(), DWORD_SIZE);
  }
  memory[addr] = val[0];
  memory[addr+1] = val[1];
  memory[addr+2] = val[2];
  memory[addr+3] = val[3];
  memory[addr+4] = val[4];
  memory[addr+5] = val[5];
  memory[addr+6] = val[6];
  memory[addr+7] = val[7];
}

bytes Memory::readDWord(ulong addr) {
  if (addr + DWORD_SIZE >= size) {
    throw new AddressOutOfMemoryException(addr, DWORD_SIZE, size, true);
  }
  return bytes {memory[addr], memory[addr+1], memory[addr+2], memory[addr+3], memory[addr+4], memory[addr+5], memory[addr+6], memory[addr+7]};
}

void Memory::writeQWord(ulong addr, bytes val) {
  if (addr + QWORD_SIZE >= size) {
    throw new AddressOutOfMemoryException(addr, QWORD_SIZE, size, false);
  }
  if (val.size() != QWORD_SIZE) {
    throw new WrongSizeInstructionException(val.size(), QWORD_SIZE);
  }
  memory[addr] = val[0];
  memory[addr+1] = val[1];
  memory[addr+2] = val[2];
  memory[addr+3] = val[3];
  memory[addr+4] = val[4];
  memory[addr+5] = val[5];
  memory[addr+6] = val[6];
  memory[addr+7] = val[7];
}

bytes Memory::readQWord(ulong addr) {
  if (addr + QWORD_SIZE >= size) {
    throw new AddressOutOfMemoryException(addr, QWORD_SIZE, size, true);
  }
  return bytes {memory[addr], memory[addr+1], memory[addr+2], memory[addr+3], memory[addr+4], memory[addr+5], memory[addr+6], memory[addr+7]};
}

ulong Memory::getSize() {
  return this->size;
}

void Memory::printRegion(ulong start, ulong count, ushort outWidth) {
  if (start > size || start + count > size) {
    throw new AddressOutOfMemoryException(start, count, size, true);
  }
  ushort widthCounter = 0;
  for (int i=start; i < start+count; i++) {
    if (widthCounter == 0) {
      printf("% 6d\t%02X ", i, memory[i]);
    } else {
      printf("%02X ", memory[i]);
    }
    widthCounter++;
    if (widthCounter == outWidth) {
      printf("\n");
      widthCounter = 0;
    }
  }
}
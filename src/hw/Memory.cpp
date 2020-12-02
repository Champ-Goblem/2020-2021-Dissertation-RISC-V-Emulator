#include "../include/hw/Memory.h"

Memory::Memory(ulong size) {
  memory = bytes(size);
  this->size = size;
}

void Memory::writeByte(ulong addr, byte val) {
  if (addr >= size) {
    throw new MemoryException("Address out of range of memory region [addr: %d, max: %d]", addr, size);
  }
  memory[addr] = val;
}

byte Memory::readByte(ulong addr) {
  if (addr >= size) {
    throw new MemoryException("Address out of range of memory region [addr: %d, max: %d]", addr, size);
  }
  return memory[addr];
}

void Memory::writeWord(ulong addr, bytes val) {
  if (addr + WORD_SIZE >= size) {
    throw new MemoryException("Write to address out of range of memory region [addr: %d, off: %d, max: %d]", addr, WORD_SIZE, size);
  }
  if (val.size() != WORD_SIZE) {
    throw new MemoryException("Wrong size write [size: %d, allowed: %d]", val.size(), WORD_SIZE);
  }
  memory[addr] = val[0];
  memory[addr+1] = val[1];
}

bytes Memory::readWord(ulong addr) {
  if (addr + WORD_SIZE >= size) {
    throw new MemoryException("Write to address out of range of memory region [addr: %d, off: %d, max: %d]", addr, WORD_SIZE, size);
  }
  return bytes {memory[addr], memory[addr+1]};
}

void Memory::writeDWord(ulong addr, bytes val) {
  if (addr + DWORD_SIZE >= size) {
    throw new MemoryException("Write to address out of range of memory region [addr: %d, off: %d, max: %d]", addr, DWORD_SIZE, size);
  }
  if (val.size() != DWORD_SIZE) {
    throw new MemoryException("Wrong size write [size: %d, allowed: %d]", val.size(), DWORD_SIZE);
  }
  memory[addr] = val[0];
  memory[addr+1] = val[1];
  memory[addr+2] = val[2];
  memory[addr+3] = val[3];
}

bytes Memory::readDWord(ulong addr) {
  if (addr + DWORD_SIZE >= size) {
    throw new MemoryException("Write to address out of range of memory region [addr: %d, off: %d, max: %d]", addr, DWORD_SIZE, size);
  }
  return bytes {memory[addr], memory[addr+1], memory[addr+2], memory[addr+3]};
}

void Memory::writeQWord(ulong addr, bytes val) {
  if (addr + QWORD_SIZE >= size) {
    throw new MemoryException("Write to address out of range of memory region [addr: %d, off: %d, max: %d]", addr, QWORD_SIZE, size);
  }
  if (val.size() != QWORD_SIZE) {
    throw new MemoryException("Wrong size write [size: %d, allowed: %d]", val.size(), QWORD_SIZE);
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
    throw new MemoryException("Write to address out of range of memory region [addr: %d, off: %d, max: %d]", addr, QWORD_SIZE, size);
  }
  return bytes {memory[addr], memory[addr+1], memory[addr+2], memory[addr+3], memory[addr+4], memory[addr+5], memory[addr+6], memory[addr+7]};
}

void Memory::printRegion(ulong start, ulong count, ushort outWidth) {
  if (start > size || start + count > size) {
    throw new MemoryException("Address out of range of memory region [addr: %d, off: %d, max: %d]", start, count, size);
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
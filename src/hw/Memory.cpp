#include "../include/hw/Memory.h"
#include "../include/exceptions.h"
#include "../include/bytemanip.h"

Memory::Memory(ulong size): lock() {
  memory = bytes(size);
  this->size = size;
}

void Memory::writeByte(ulong addr, byte val) {
  lock_guard<mutex> lck(lock);
  if (addr >= size) {
    throw AddressOutOfMemoryException(addr, 0, size, false);
  }
  memory[addr] = val;
}

byte Memory::readByte(ulong addr) {
  lock_guard<mutex> lck(lock);
  if (addr >= size) {
    throw AddressOutOfMemoryException(addr, 0, size, true);
  }
  return memory[addr];
}

void Memory::writeHWord(ulong addr, bytes val) {
  lock_guard<mutex> lck(lock);
  if (addr + HWORD_SIZE >= size) {
    throw AddressOutOfMemoryException(addr, HWORD_SIZE, size, false);
  }
  if (val.size() != HWORD_SIZE) {
    throw WrongSizeInstructionException(val.size(), HWORD_SIZE);
  }
  memory[addr] = val[0];
  memory[addr+1] = val[1];
}

bytes Memory::readHWord(ulong addr) {
  lock_guard<mutex> lck(lock);
  if (addr + HWORD_SIZE >= size) {
    throw AddressOutOfMemoryException(addr, HWORD_SIZE, size, true);
  }
  return bytes {memory[addr], memory[addr+1]};
}

void Memory::writeWord(ulong addr, bytes val) {
  lock_guard<mutex> lck(lock);
  if (addr + WORD_SIZE >= size) {
    throw AddressOutOfMemoryException(addr, WORD_SIZE, size, false);
  }
  if (val.size() != WORD_SIZE) {
    throw WrongSizeInstructionException(val.size(), WORD_SIZE);
  }
  memory[addr] = val[0];
  memory[addr+1] = val[1];
  memory[addr+2] = val[2];
  memory[addr+3] = val[3];
}

bytes Memory::readWord(ulong addr) {
  lock_guard<mutex> lck(lock);
  if (addr + WORD_SIZE >= size) {
    throw AddressOutOfMemoryException(addr, WORD_SIZE, size, true);
  }
  return bytes {memory[addr], memory[addr+1], memory[addr+2], memory[addr+3]};
}

void Memory::writeDWord(ulong addr, bytes val) {
  lock_guard<mutex> lck(lock);
  if (addr + DWORD_SIZE >= size) {
    throw AddressOutOfMemoryException(addr, DWORD_SIZE, size, false);
  }
  if (val.size() != DWORD_SIZE) {
    throw WrongSizeInstructionException(val.size(), DWORD_SIZE);
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
  lock_guard<mutex> lck(lock);
  if (addr + DWORD_SIZE >= size) {
    throw AddressOutOfMemoryException(addr, DWORD_SIZE, size, true);
  }
  return bytes {memory[addr], memory[addr+1], memory[addr+2], memory[addr+3], 
    memory[addr+4], memory[addr+5], memory[addr+6], memory[addr+7]};
}

void Memory::writeQWord(ulong addr, bytes val) {
  lock_guard<mutex> lck(lock);
  if (addr + QWORD_SIZE >= size) {
    throw AddressOutOfMemoryException(addr, QWORD_SIZE, size, false);
  }
  if (val.size() != QWORD_SIZE) {
    throw WrongSizeInstructionException(val.size(), QWORD_SIZE);
  }
  memory[addr] = val[0];
  memory[addr+1] = val[1];
  memory[addr+2] = val[2];
  memory[addr+3] = val[3];
  memory[addr+4] = val[4];
  memory[addr+5] = val[5];
  memory[addr+6] = val[6];
  memory[addr+7] = val[7];
  memory[addr+8] = val[8];
  memory[addr+9] = val[9];
  memory[addr+10] = val[10];
  memory[addr+11] = val[11];
  memory[addr+12] = val[12];
  memory[addr+13] = val[13];
  memory[addr+14] = val[14];
  memory[addr+15] = val[15];
}

bytes Memory::readQWord(ulong addr) {
  lock_guard<mutex> lck(lock);
  if (addr + QWORD_SIZE >= size) {
    throw AddressOutOfMemoryException(addr, QWORD_SIZE, size, true);
  }
  return bytes {memory[addr], memory[addr+1], memory[addr+2], memory[addr+3], 
    memory[addr+4], memory[addr+5], memory[addr+6], memory[addr+7],
    memory[addr+8], memory[addr+9], memory[addr+10], memory[addr+11],
    memory[addr+12], memory[addr+13], memory[addr+14], memory[addr+15]};
}

ulong Memory::getSize() {
  return this->size;
}

vector<byte> Memory::getRegion(ulong start, ulong count) {
  lock_guard<mutex> lck(lock);
  if (start > size || start + count > size) {
    throw AddressOutOfMemoryException(start, count, size, true);
  }
  bytes out;
  for (uint i=start; i < count; i++) {
    out.push_back(memory[i]);
  }
  return out;
}

void Memory::printRegion(ulong start, ulong count, ushort outWidth) {
  lock_guard<mutex> lck(lock);
  if (start > size || start + count > size) {
    throw AddressOutOfMemoryException(start, count, size, true);
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
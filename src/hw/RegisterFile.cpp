#include "../include/hw/RegisterFile.h"
#include "../include/bytemanip.h"

RegisterFile::RegisterFile(ushort XLEN, bool isRV32E): lock() {
  int registerCount = isRV32E ? REG16 : REG32;
  this->RV32E = isRV32E;
  this->XLEN = XLEN;
  for (int i=0; i<registerCount; i++) {
    registerMap.insert(make_pair((ushort)i, bytes(XLEN)));
  }
  // pc = bytes(XLEN);
}

bytes RegisterFile::get(ushort reg) {
  lock_guard<mutex> lck(lock);
  if (reg == 0) {
    // Return constant zero
    return bytes(XLEN);
  }
  if (reg > (RV32E ? REG16 : REG32)) {
    // Check size and throw error
    throw new RegisterFileException("Failed to read from register x%d, [min: %d, max %d]\n", reg, 0, RV32E ? REG16 : REG32);
  }
  RegisterIterator itr = registerMap.find(reg);
  if (itr == registerMap.end()) {
    // Check if for some reason the register doesnt exist in the map
    throw new RegisterFileException("[FATAL]: Failed to read from register x%d, couldnt find register in map\n", reg);
  }
  return itr->second;
}

void RegisterFile::write(ushort reg, bytes val) {
  lock_guard<mutex> lck(lock);
  if (reg == 0) {
    // Ignore register write to zero
    return;
  }
  if (val.size() != XLEN) {
    // Check size of value being written and throw
    throw new RegisterFileException("Failed to write to register x%d, size of value to write bigger than XLEN [s: %d, XLEN: %d]\n", reg, val.size(), XLEN);
  }
  if (reg > (RV32E ? REG16 : REG32)) {
    // Check size and throw
    throw new RegisterFileException("Failed to write to register x%d, [min: %d, max %d]\n", reg, 0, RV32E ? REG16 : REG32);
  }
  RegisterIterator itr = registerMap.find(reg);
  if (itr == registerMap.end()) {
    // Check if for some reason the register doesnt exist in the map
    throw new RegisterFileException("[FATAL]: Failed to write to register x%d, couldnt find register in map\n", reg);
  }
  itr->second = val;
}

// void RegisterFile::writePC(bytes addr) {
//   if (addr.size() != XLEN) {
//     throw new RegisterFileException("Failed to write to PC, size of value to write bigger than XLEN [s: %d, XLEN: %d]\n", addr.size(), XLEN);
//   }
//   pc = addr;
// }

// bytes RegisterFile::getPC() {
//   return pc;
// }

// void RegisterFile::incPC() {
//   // TODO: 4 needs changing to allow for C ext
//   this->pc = addByteToBytes(this->pc, 4);
// }

void RegisterFile::debug() {
  lock_guard<mutex> lck(lock);
  // Print the current register values
  RegisterIterator itr = registerMap.begin();
  while (itr != registerMap.end()) {
    printf("% 2u: ", itr->first);
    bytes val = itr->second;
    for (int i=0; i<XLEN; i++) {
      printf("%02X ", val[XLEN - 1 - i]);
    }
    printf("\n");
    itr++;
  }
  // printf("\n");
  // printf("PC: ");
  // for (int i=0; i<XLEN; i++) {
  //   printf("%02X ", pc[XLEN - 1 - i]);
  // }
  printf("\n");
}
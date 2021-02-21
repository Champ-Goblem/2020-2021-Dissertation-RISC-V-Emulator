#include "../include/hw/RegisterFile.h"
#include "../include/bytemanip.h"

RegisterFile::RegisterFile(ushort XLEN, bool isRV32E): lock() {
  int registerCount = isRV32E ? REG16 : REG32;
  this->RV32E = isRV32E;
  this->XLEN = XLEN;
  for (int i=0; i<registerCount; i++) {
    registerMap.insert(make_pair((ushort)i, bytes(XLEN)));
  }
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

vector<bytes> RegisterFile::debug() {
  lock_guard<mutex> lck(lock);
  // Print the current register values
  RegisterIterator itr = registerMap.begin();
  vector<bytes> out;
  while (itr != registerMap.end()) {
    out.push_back(itr->second);
    itr++;
  }
  return out;
}
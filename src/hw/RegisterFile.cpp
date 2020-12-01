#include "../include/hw/RegisterFile.h"

RegisterFile::RegisterFile(ushort XLEN, bool isRV32E) {
  int registerCount = isRV32E ? REG16 : REG32;
  this->RV32E = isRV32E;
  this->XLEN = XLEN;
  for (int i=0; i<registerCount; i++) {
    registerMap.insert(make_pair((ushort)i, bytes {0,0,0,0}));
  }
}

bytes RegisterFile::get(ushort reg) {
  return bytes {0,0,0,0};
}

void RegisterFile::write(ushort reg, bytes val) {
  if (reg > (RV32E ? REG16 : REG32)) {
    throw new exception();
  }
  RegisterIterator itr = registerMap.find(reg);
  itr->second = val;
}

void RegisterFile::debug() {
  int registerCount = RV32E ? 16 : 32;
  byte* zero = new byte[XLEN];
  RegisterIterator itr = registerMap.begin();
  while (itr != registerMap.end()) {
    printf("x%u: ", itr->first);
    bytes val = itr->second;
    for (int i=0; i<XLEN; i++) {
      printf("%x ", val[i]);
    }
    printf("\n");
    itr++;
  }
}
#ifndef __Exceptions__
#define __Exceptions__

#include "bytemanip.h"
#include "emustd.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

class AddressMisalignedException: public EmulatorException {
  private:
  bytes prevPC;
  bytes nextPC;
  bytes instruction;

  public:
  AddressMisalignedException(): EmulatorException() {};
  AddressMisalignedException(bytes prevPC, bytes nextPC, bytes instruction, string message): EmulatorException(message) {
    this->prevPC = prevPC;
    this->nextPC = nextPC;
    this->instruction = instruction;
  };

  string getMessage() {
    ostringstream str;
    str << "Calculated PC value not 4-bytes aligned\n";
    str << "Message: " << this->EmulatorException::getMessage();
    str << "Instruction: " << getBytesForPrint(instruction) << "\n";
    str << "\t" << "Current PC: " << getBytesForPrint(prevPC) << "\n";
    str << "\t" << "Calculated PC: " << getBytesForPrint(nextPC) << "\n";
    return str.str();
  }
};

class AddressOutOfMemoryException: public EmulatorException {
  public:
  AddressOutOfMemoryException(): EmulatorException() {};
  AddressOutOfMemoryException(ulong addr, ulong off, ulong size, bool isRead, string message=""): EmulatorException() {
    ostringstream str;
    str << (isRead ? "Read" : "Write") << " to address out of range of memory region\n";
    str << "Message: " << message << "\n";
    str << "\tAddress: " << addr << "\n";
    str << "\tOffset: " << off << " Bytes\n";
    str << "\tMemory Size: " << size << " Bytes\n";
    this->message = str.str();
  }
};

class WrongSizeInstructionException: public EmulatorException {
  public:
  WrongSizeInstructionException(): EmulatorException() {};
  WrongSizeInstructionException(ushort size, ushort requiredSize): EmulatorException() {
    ostringstream str;
    str << "Wrong size instruction [size: " << size << ", required: " << requiredSize << "]\n";
    this->message = str.str();
  }
};

#endif
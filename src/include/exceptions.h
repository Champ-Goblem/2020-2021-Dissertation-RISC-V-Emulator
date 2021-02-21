#ifndef __Exceptions__
#define __Exceptions__

#include "bytemanip.h"
#include "emustd.h"
#include "instructions/AbstractInstruction.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

class AddressMisalignedException: public EmulatorException {
  public:
  AddressMisalignedException(): EmulatorException() {};
  AddressMisalignedException(bytes prevPC, bytes nextPC, bytes instruction, string message): EmulatorException() {
    ostringstream str;
    str << "Calculated PC value not 4-bytes aligned\n";
    str << "Message: " << this->EmulatorException::getMessage();
    str << "Instruction: " << getBytesForPrint(instruction) << "\n";
    str << "\t" << "Current PC: " << getBytesForPrint(prevPC) << "\n";
    str << "\t" << "Calculated PC: " << getBytesForPrint(nextPC) << "\n";
    this->message = str.str();
  };
  AddressMisalignedException(AbstractInstruction* instruction, string message=""): EmulatorException() {
    ostringstream str;
    str << "Calculated PC value not 4-bytes aligned\n";
    str << "Message: " << this->EmulatorException::getMessage();
    str << "\t" << "Current PC: " << getBytesForPrint(instruction->getPC()) << "\n";
    str << "\t" << "Calculated PC: " << getBytesForPrint(instruction->getResult()) << "\n";
    str << instruction->debug() << "\n";
    this->message = str.str();
  };
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

class UndefinedInstructionException: public EmulatorException {
  public:
  UndefinedInstructionException(): EmulatorException() {};
  UndefinedInstructionException(AbstractInstruction* instruction, string message=""): EmulatorException() {
    ostringstream str;
    str << "Undefined instruction" << "\n";
    str << "Message: " << message << "\n";
    str << instruction->debug() << "\n";
    this->message = str.str();
  };
};

class FailedBranchPredictionException: public EmulatorException {
  public:
  FailedBranchPredictionException(): EmulatorException() {};
  FailedBranchPredictionException(AbstractInstruction* instruction, string message=""): EmulatorException() {
    ostringstream str;
    str << "Branch prediction failed to predict correctly, resetting the pipeline\n";
    str << "Message: " << message << "\n";
    str << "\tPC: " << getBytesForPrint(instruction->getPC()) << "\n";
    str << "\tTarget: " << getBytesForPrint(instruction->getResult()) << "\n";
    this->message = str.str();
  };
};

class FailedToLoadProgramException: public EmulatorException {
  public:
  FailedToLoadProgramException(): EmulatorException() {};
  FailedToLoadProgramException(string filePath, string message=""): EmulatorException() {
    ostringstream oss;
    oss << "Failed to load binary into memory" << endl;
    oss << "Message: " << message << endl;
    oss << "\tPath: " << filePath << endl;
    this->message = oss.str();
  }
};

class FailedDebugException: public EmulatorException {
  public:
  FailedDebugException(): EmulatorException() {};
  FailedDebugException(string message=""): EmulatorException() {
    ostringstream oss;
    oss << "Failed to debug" << endl;
    oss << "Message: " << message << endl;
    this->message = oss.str();
  }
};

#endif
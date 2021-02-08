#include "../include/hw/Hart.h"
#include "../include/hw/Memory.h"
#include "../include/hw/RegisterFile.h"
#include "../include/units/SimpleBranchPredictor.h"
#include "../include/bytemanip.h"
#include <exception>

Hart::Hart(Memory* memory, RegisterFile* registerFile, AbstractISA baseISA, ExtensionSet extensions, ushort XLEN, bytes initialPC, bool isRV32E): pipelineController(XLEN, registerFile, isRV32E) {
  if (XLEN == 0) {
    throw new EmulatorException("Failed to create hart, XLEN is 0");
  }
  this->memory = memory;
  this->registerFile = registerFile;
  this->baseISA = baseISA;
  this->extensions = extensions;
  this->XLEN = XLEN;
  vector<struct OpcodeSpace> opcodeSpace = baseISA.registerOpcodeSpace();

  for (ExtensionSet::iterator it = extensions.begin(); it != extensions.end(); ++it) {
    vector<struct OpcodeSpace> extensionSpace = it->registerOpcodeSpace();
    opcodeSpace.insert(opcodeSpace.end(), extensionSpace.begin(), extensionSpace.end());
    // TODO: Could do a check to see if an opcode is already defined
  }

  if (opcodeSpace.size() == 0) {
    throw new EmulatorException("Failed to create hart, no instructions defined");
  }

  this->opcodeSpace = opcodeSpace;
  this->branchPredictor = SimpleBranchPredictor(memory, XLEN, registerFile, initialPC);
}

// void Hart::startExecution() {
//   controlThread = thread(&Hart::controller, this);
// }

void Hart::tick() {
  exception_ptr fetchException, decodeException, executeException, memException, wbException;
  AbstractInstruction newlyDecoded;

  // Reset stall flag
  stall = false;

  // If we need to stall due to a dependecy we dont want to
  // fetch again otherwise that will poll branch predictor
  // causing it to become out of sync
  thread fetch;
  if (!stall) {
    fetch = thread(&Hart::fetch, this, fetchException, fetchPC, fromFetch);
  }
  thread decode(&Hart::decode, this, fromFetch, fetchPC, decodeException, newlyDecoded);
  thread execute(&Hart::execute, this, &fromDecode, executeException);
  thread memoryAccess(&Hart::memoryAccess, this, &fromExecute, memException);
  thread writeback(&Hart::writeback, this, &fromMem, wbException);

  if (!stall) {
    fetch.join();
  }
  decode.join();
  execute.join();
  memoryAccess.join();
  writeback.join();

  fromMem = fromExecute;
  fromExecute = fromDecode;
  fromDecode = newlyDecoded;

  if (fetchException) {
    rethrow_exception(fetchException);
  }
  if (decodeException) {
    rethrow_exception(decodeException);
  }
  if (executeException) {
    rethrow_exception(executeException);
  }
  if (memException) {
    rethrow_exception(memException);
  }
  if (wbException) {
    rethrow_exception(wbException);
  }
}

void Hart::fetch(exception_ptr exception, bytes outPC, bytes outInstruction) {
  try {
    outPC = branchPredictor.getNextPC();
    outInstruction = memory->readWord(getBytesToULong(outPC));
  } catch (...) { 
    exception = current_exception();
  }
}

void Hart::decode(bytes instruction, bytes pc, exception_ptr exception, AbstractInstruction outInstruction) {
  try {
    byte opcode = instruction[0] & 127;
    DecodeRoutine decodeRoutine = AbstractISA::findDecodeRoutineByOpcode(opcodeSpace, opcode);
    AbstractInstruction inst = decodeRoutine(instruction, &pipelineController, stall);
    inst.setPC(pc);
    outInstruction = inst;
  } catch (...) {
    exception = current_exception();
  }
}

void Hart::execute(AbstractInstruction* instruction, exception_ptr exception) {
  try {
    if (instruction->execute) {
      instruction->execute(instruction, &branchPredictor, memory->getSize(), &pipelineController);
    }
  } catch (...) {
    exception = current_exception();
  }
}

void Hart::memoryAccess(AbstractInstruction* instruction, exception_ptr exception) {
  try {
    if (instruction->memoryAccess) {
      instruction->memoryAccess(instruction, memory, &pipelineController);
    }
  } catch (...) {
    exception = current_exception();
  }
}

void Hart::writeback(AbstractInstruction* instruction, exception_ptr exception) {
  try {
    if (instruction->registerWriteback) {
      instruction->registerWriteback(instruction, registerFile);
    }
  } catch (...) {
    exception = current_exception();
  }
}
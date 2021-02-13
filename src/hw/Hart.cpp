#include <exception>

#include "../include/hw/Hart.h"
#include "../include/hw/Memory.h"
#include "../include/hw/RegisterFile.h"
#include "../include/units/SimpleBranchPredictor.h"
#include "../include/bytemanip.h"
#include "../include/exceptions.h"
#include "../include/instructions/RType.h"

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
  this->branchPredictor = new SimpleBranchPredictor(memory, XLEN, registerFile, initialPC);
}

void Hart::tick() {
  if (stall) cout << "stall\n";
  cout << getBytesForPrint(branchPredictor->peak()) << "\t" << getBytesForPrint(decodePC) << "\t" << getBytesForPrint(toExecute.getPC()) << "\t"
    << getBytesForPrint(toMem.getPC()) << "\t" << getBytesForPrint(toWB.getPC()) << "\n";
  // Reset exception pointers
  fetchException = nullptr;
  decodeException = nullptr;
  executeException = nullptr;
  memException = nullptr;
  wbException = nullptr;
  
  // Reset stall flag
  stallNextTick = false;
  // Reset failed prediction
  failedPrediction = false;

  // Bump the controller so all instruction are
  // in the correct spot for the coming cycle
  // and so that the new instruction can be added
  // when decoded
  pipelineController.bump();

  // If we need to stall due to a dependecy we dont want to
  // fetch again otherwise that will poll branch predictor
  // causing it to become out of sync
  thread fetch, decode, execute, memoryAccess, writeback;
  if (!stall) {
    fetch = thread(&Hart::fetch, this);
  }
  if (toDecode.size() > 0) {
    decode = thread(&Hart::decode, this, toDecode, decodePC);
  }
  if (toExecute.getXLEN() > 0) {
    execute = thread(&Hart::execute, this, &toExecute);
  }
  if (toMem.getXLEN() > 0) {
    memoryAccess = thread(&Hart::memoryAccess, this, &toMem);
  }
  if (toWB.getXLEN() > 0) {
    writeback = thread(&Hart::writeback, this, &toWB);
  }

  if (!stall) {
    fetch.join();
  }
  if (toDecode.size() > 0) {
    decode.join();
  }
  if (toExecute.getXLEN() > 0) {
    execute.join();
  }
  if (toMem.getXLEN() > 0) {
    memoryAccess.join();
  }
  if (toWB.getXLEN() > 0) {
    writeback.join();
  }

  toWB = fromMem;
  toMem = fromExecute;
  toExecute = failedPrediction ? NOP(XLEN) : fromDecode;
  if (!stallNextTick) {
    toDecode = fromFetch;
    decodePC = fetchPC;
  }
  if (failedPrediction) {
    // As we failed prediction we need to set current decode
    // to NOP and set next to be decoded to NOP
    pipelineController.enqueue(NOP(XLEN));
    toDecode = NOP_BYTES;
    decodePC = bytes(0);
  }

  stall = stallNextTick;

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

void Hart::fetch() {
  try {
    this->fetchPC = branchPredictor->getNextPC();
    this->fromFetch = memory->readWord(getBytesToULong(fetchPC));
  } catch (...) { 
    this->fetchException = current_exception();
  }
}

void Hart::decode(bytes instruction, bytes pc) {
  try {
    byte opcode = instruction[0] & 127;
    DecodeRoutine decodeRoutine = AbstractISA::findDecodeRoutineByOpcode(opcodeSpace, opcode);
    AbstractInstruction inst = decodeRoutine(instruction, &pipelineController, &stallNextTick);
    if (!stallNextTick) {
      inst.setPC(pc);
    }
    this->fromDecode = inst;
  } catch (...) {
    this->decodeException = current_exception();
  }
}

void Hart::execute(AbstractInstruction* instruction) {
  try {
    if (instruction->execute) {
      instruction->execute(instruction, branchPredictor, memory->getSize(), &pipelineController);
    }
    this->fromExecute = *instruction;
  } catch (FailedBranchPredictionException e) {
    cerr << e.getMessage();
    this->failedPrediction = true;
  } catch (...) {
    this->executeException = current_exception();
  }
}

void Hart::memoryAccess(AbstractInstruction* instruction) {
  try {
    if (instruction->memoryAccess) {
      instruction->memoryAccess(instruction, memory, &pipelineController);
    }
    this->fromMem = *instruction;
  } catch (...) {
    this->memException = current_exception();
  }
}

void Hart::writeback(AbstractInstruction* instruction) {
  try {
    if (instruction->registerWriteback) {
      instruction->registerWriteback(instruction, registerFile);
    }
  } catch (...) {
    this->wbException = current_exception();
  }
}

Hart::~Hart() {
  delete(this->branchPredictor);
}
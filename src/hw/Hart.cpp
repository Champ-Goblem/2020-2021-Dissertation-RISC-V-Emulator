#include <exception>

#include "../include/hw/Hart.h"
#include "../include/hw/Memory.h"
#include "../include/hw/RegisterFile.h"
#include "../include/units/SimpleBranchPredictor.h"
#include "../include/bytemanip.h"
#include "../include/exceptions.h"
#include "../include/instructions/RType.h"
#include "../include/instructions/sets/RV32I.h"
#include "../include/units/SimpleBranchPredictor.h"

Hart::Hart(Memory* memory, Bases baseISA, vector<Extensions> extensions, BranchPredictors branchPredictor, ushort XLEN, bytes initialPC, bool isRV32E): registerFile(XLEN, isRV32E), pipelineController(XLEN, &registerFile, isRV32E) {
  if (XLEN == 0) {
    throw new EmulatorException("Failed to create hart, XLEN is 0");
  }
  this->memory = memory;
  this->baseISA = getBase(baseISA);
  this->extensions = getExtensions(extensions);
  this->XLEN = XLEN;
  vector<struct OpcodeSpace> opcodeSpace = this->baseISA.registerOpcodeSpace();

  for (ExtensionSet::iterator it = this->extensions.begin(); it != this->extensions.end(); ++it) {
    vector<struct OpcodeSpace> extensionSpace = it->registerOpcodeSpace();
    opcodeSpace.insert(opcodeSpace.end(), extensionSpace.begin(), extensionSpace.end());
    // TODO: Could do a check to see if an opcode is already defined
  }

  if (opcodeSpace.size() == 0) {
    throw new EmulatorException("Failed to create hart, no instructions defined");
  }

  this->opcodeSpace = opcodeSpace;
  this->branchPredictor = getBranchPredictor(branchPredictor, memory, XLEN, &registerFile, initialPC);
}

void Hart::tick(exception_ptr& exception) {
  // if (stall) cout << "stall\n";
  // cout << getBytesForPrint(branchPredictor->peak()) << "\t" << getBytesForPrint(decodePC) << "\t" << getBytesForPrint(toExecute.getPC()) << "\t"
  //   << getBytesForPrint(toMem.getPC()) << "\t" << getBytesForPrint(toWB.getPC()) << "\n";
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
    exception = fetchException;
  }
  if (decodeException) {
    exception = decodeException;
  }
  if (executeException) {
    exception = executeException;
  }
  if (memException) {
    exception = memException;
  }
  if (wbException) {
    exception = wbException;
  }
}

void Hart::fetch() {
  try {
    this->fetchPC = this->branchPredictor->getNextPC();
    this->fromFetch = this->memory->readWord(getBytesToULong(fetchPC));
  } catch (...) { 
    this->fetchException = current_exception();
  }
}

void Hart::decode(bytes instruction, bytes pc) {
  try {
    byte opcode = instruction[0] & 127;
    DecodeRoutine decodeRoutine = AbstractISA::findDecodeRoutineByOpcode(opcodeSpace, opcode);
    AbstractInstruction inst = decodeRoutine(instruction, &this->pipelineController, &this->stallNextTick);
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
      instruction->execute(instruction, this->branchPredictor, this->memory->getSize(), &this->pipelineController);
    }
    this->fromExecute = *instruction;
  } catch (FailedBranchPredictionException e) {
    this->failedPrediction = true;
  } catch (...) {
    this->executeException = current_exception();
  }
}

void Hart::memoryAccess(AbstractInstruction* instruction) {
  try {
    if (instruction->memoryAccess) {
      instruction->memoryAccess(instruction, this->memory, &this->pipelineController);
    }
    this->fromMem = *instruction;
  } catch (...) {
    this->memException = current_exception();
  }
}

void Hart::writeback(AbstractInstruction* instruction) {
  try {
    if (instruction->registerWriteback) {
      instruction->registerWriteback(instruction, &this->registerFile);
    }
  } catch (...) {
    this->wbException = current_exception();
  }
}

AbstractISA Hart::getBase(Bases base) {
  switch (base) {
    case Bases::RV32IBase:
      return RV32I();
  }
};

ExtensionSet Hart::getExtensions(vector<Extensions> extensions) {
  ExtensionSet exts = ExtensionSet(0);

  for (uint i=0; i < extensions.size(); i++) {
    switch (extensions[i]) {};
  }
  return exts;
};

AbstractBranchPredictor* Hart::getBranchPredictor(BranchPredictors branchPredictor, Memory* memory, ushort XLEN, RegisterFile* registerFile, bytes initialPC) {
  switch (branchPredictor) {
    case BranchPredictors::Simple:
      return new SimpleBranchPredictor(memory, XLEN, registerFile, initialPC);
  }
};

vector<bytes> Hart::debug(DEBUG debug) {
  switch (debug) {
    case GET_PIPELINE: {
      return vector<bytes>{branchPredictor->peak(), toDecode, toExecute.getPC(), toMem.getPC(), toWB.getPC()};
    }
    case GET_REGISTERS: {
      return registerFile.debug();
    }
  }
}

Hart::~Hart() {
  delete(this->branchPredictor);
}
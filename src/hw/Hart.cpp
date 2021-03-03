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

Hart::Hart(Memory* memory, Bases baseISA, vector<Extensions> extensions, BranchPredictors branchPredictor, ushort XLEN, bytes initialPC, bool isRV32E, bytes haltAddr): 
  registerFile(XLEN, isRV32E), pipelineController(XLEN, &registerFile, isRV32E) {
  if (XLEN == 0) {
    throw new EmulatorException("Failed to create hart, XLEN is 0");
  }
  this->memory = memory;
  this->baseISA = getBase(baseISA);
  this->extensions = getExtensions(extensions);
  this->XLEN = XLEN;
  this->haltAddr = haltAddr;
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
  if (!stall && !shouldFlush) {
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

  if (!stall && !shouldFlush) {
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
  } else if (shouldFlush) {
    // Feed the pipeline with nops if we are flushing
    decodePC = bytes(0);
    toDecode = NOP_BYTES;
  }

  // If halt has been set and current pc is greater than or equal to the haltAddr
  if (!willHalt && haltAddr.size() > 0 && bytesGreaterOrequalToUnsigned(decodePC, haltAddr)) {
    // We have hit or exceeded the haltAddr
    // We can flush the pipeline using the flush function
    // if no exception is generated we can then send back
    // HaltedProcessor exception to signal the processor was halted
    shouldFlush = true;
    willHalt = true;
    // Nop the fetched instruction so its not executed
    toDecode = NOP_BYTES;
    decodePC = bytes(0);
    exception_ptr ep;
    // Flush the current pipeline
    flush(ref(ep));
    if (ep) {
      exception = ep;
    } else {
      // Throw HaltedProcessor so we can intercept it and pass it back to the caller
      try {
        throw HaltedProcessor();
      } catch (...) {
        exception = current_exception();
      }
    }

    return;
  }

  stall = stallNextTick;

  // Check that no exceptions were generated this cycle
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

void Hart::flush(exception_ptr & exception) {
  shouldFlush = true;
  for (ushort i=0; i < 4; i++) {
    exception_ptr a;

    tick(ref(a));

    if (a) {
      exception = a;
      return;
    }
  }
  shouldFlush = false;
}

// Perform the fetch
void Hart::fetch() {
  try {
    this->fetchPC = this->branchPredictor->getNextPC();
    this->fromFetch = this->memory->readWord(getBytesToULong(fetchPC));
  } catch (...) { 
    this->fetchException = current_exception();
  }
}

// Perform the decode
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

// Perform the execute
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

// Perform the memoryAccess
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

// Perform the register writeback
void Hart::writeback(AbstractInstruction* instruction) {
  try {
    if (instruction->registerWriteback) {
      instruction->registerWriteback(instruction, &this->registerFile);
    }
  } catch (...) {
    this->wbException = current_exception();
  }
}

// Convert enum to a class for the base architecture
AbstractISA Hart::getBase(Bases base) {
  switch (base) {
    case Bases::RV32IBase:
      return RV32I();
  }
};

// Convert an enum array to a set of classes for the extensions
ExtensionSet Hart::getExtensions(vector<Extensions> extensions) {
  ExtensionSet exts = ExtensionSet(0);

  for (uint i=0; i < extensions.size(); i++) {
    switch (extensions[i]) {};
  }
  return exts;
};

// Get the branch predictor class from an enum
AbstractBranchPredictor* Hart::getBranchPredictor(BranchPredictors branchPredictor, Memory* memory, ushort XLEN, RegisterFile* registerFile, bytes initialPC) {
  switch (branchPredictor) {
    case BranchPredictors::Simple:
      return new SimpleBranchPredictor(memory, XLEN, registerFile, initialPC);
  }
};

// Debug states in the hart
vector<bytes> Hart::debug(DEBUG debug) {
  switch (debug) {
    case GET_PIPELINE: {
      return vector<bytes>{branchPredictor->peak(), decodePC, toExecute.getPC(), toMem.getPC(), toWB.getPC()};
    }
    case GET_REGISTERS: {
      return registerFile.debug();
    }
  }
}

Hart::~Hart() {
  delete(this->branchPredictor);
}
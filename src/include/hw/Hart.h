#ifndef __Hart__
#define __Hart__

#include "../emustd.h"
#include "../units/AbstractBranchPredictor.h"
#include "../units/PipelineHazardController.h"
#include "../instructions/sets/AbstractISA.h"
#include "../instructions/AbstractInstruction.h"

#define NOP_BYTES bytes{0x33, 0, 0, 0}

typedef vector<AbstractISA> ExtensionSet;

class Memory;
class RegisterFile;

class Hart {
  private:
  AbstractBranchPredictor* branchPredictor;
  Memory* memory;
  RegisterFile* registerFile;
  PipelineHazardController pipelineController;
  AbstractISA baseISA;
  ExtensionSet extensions;
  ushort XLEN;
  vector<OpcodeSpace> opcodeSpace;

  bytes fromFetch, fetchPC, toDecode, decodePC;
  AbstractInstruction fromDecode, fromExecute, fromMem, toExecute, toMem, toWB;
  exception_ptr fetchException, decodeException, executeException, memException, wbException;

  bool stall = false, stallNextTick = false, failedPrediction = false;

  public:
  Hart(Memory* memory, RegisterFile* registerFile, AbstractISA baseISA, ExtensionSet extensions, ushort XLEN, bytes initialPC, bool isRV32E);
  ~Hart();
  // void startExecution();
  void tick();
  
  private:
  void fetch();
  void decode(bytes instruction, bytes pc);
  void execute(AbstractInstruction* instruction);
  void memoryAccess(AbstractInstruction* instruction);
  void writeback(AbstractInstruction* instruction);
};

#endif
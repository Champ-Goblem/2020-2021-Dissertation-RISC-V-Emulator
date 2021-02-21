#ifndef __Hart__
#define __Hart__

#include "../emustd.h"
#include "../units/AbstractBranchPredictor.h"
#include "../units/PipelineHazardController.h"
#include "../instructions/sets/AbstractISA.h"
#include "../instructions/AbstractInstruction.h"
#include "../hw/RegisterFile.h"

#define NOP_BYTES bytes{0x33, 0, 0, 0}

typedef vector<AbstractISA> ExtensionSet;

enum DEBUG {
  GET_PIPELINE,
  GET_REGISTERS
};

class Memory;
class Hart {
  private:
  AbstractBranchPredictor* branchPredictor;
  Memory* memory;
  RegisterFile registerFile;
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
  Hart(Memory* memory, Bases baseISA, vector<Extensions> extensions, BranchPredictors branchPredictor, ushort XLEN, bytes initialPC, bool isRV32E);
  ~Hart();
  void tick(exception_ptr & exception);
  vector<bytes> debug(DEBUG debug);
  
  private:
  void fetch();
  void decode(bytes instruction, bytes pc);
  void execute(AbstractInstruction* instruction);
  void memoryAccess(AbstractInstruction* instruction);
  void writeback(AbstractInstruction* instruction);
  AbstractISA getBase(Bases base);
  ExtensionSet getExtensions(vector<Extensions> extensions);
  AbstractBranchPredictor* getBranchPredictor(BranchPredictors branchPredictor, Memory* memory, ushort XLEN, RegisterFile* registerFile, bytes initialPC);
};

#endif
#ifndef __Hart__
#define __Hart__

#include "../emustd.h"
#include "../units/AbstractBranchPredictor.h"
#include "../units/PipelineHazardController.h"
#include "../instructions/sets/AbstractISA.h"
#include "../instructions/AbstractInstruction.h"

typedef vector<AbstractISA> ExtensionSet;

class Memory;
class RegisterFile;

class Hart {
  private:
  AbstractBranchPredictor branchPredictor;
  Memory* memory;
  RegisterFile* registerFile;
  PipelineHazardController pipelineController;
  AbstractISA baseISA;
  ExtensionSet extensions;
  ushort XLEN;
  vector<OpcodeSpace> opcodeSpace;
  thread controlThread;

  bytes fromFetch, fetchPC;
  AbstractInstruction fromDecode, fromExecute, fromMem;

  bool stall;

  public:
  Hart(Memory* memory, RegisterFile* registerFile, AbstractISA baseISA, ExtensionSet extensions, ushort XLEN, bytes initialPC, bool isRV32E);
  // void startExecution();
  void tick();
  
  private:
  void fetch(exception_ptr exception, bytes outPC, bytes outInstruction);
  void decode(bytes instruction, bytes pc, exception_ptr exception, AbstractInstruction outInstruction);
  void execute(AbstractInstruction* instruction, exception_ptr exception);
  void memoryAccess(AbstractInstruction* instruction, exception_ptr exception);
  void writeback(AbstractInstruction* instruction, exception_ptr exception);
};

#endif
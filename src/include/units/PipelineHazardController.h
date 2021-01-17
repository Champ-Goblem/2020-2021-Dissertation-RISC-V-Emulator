#ifndef __PipelineHazardController__
#define __PipelineHazardController__

#include "../emustd.h"
#include "AbstractUnit.h"
// #include "../instructions/AbstractInstruction.h"
// #include "../hw/RegisterFile.h"

// reverse order as we push values on to the back and remove from the front
#define STAGE_DECODE 3
#define STAGE_EXECUTE 2
#define STAGE_MEM 1
#define STAGE_WB 0

#define NUM_STAGES 4

class RegisterFile;
class AbstractInstruction;

class PipelineHazardException: public EmulatorException {
  public:
  PipelineHazardException(): EmulatorException(){};
  PipelineHazardException(string message): EmulatorException(message) {};
};

struct InstructionQueueEntry {
  AbstractInstruction* instruction;
  bytes RDVal;
};

class PipelineHazardController: public AbstractUnit {
  private:
  // length 4, positions defined by STAGE_... above
  vector<InstructionQueueEntry> executingQueue = vector<InstructionQueueEntry>(NUM_STAGES);
  ushort XLEN;
  RegisterFile* registerFile;
  bool isRV32E;
  ushort usedQueueSize = 0;

  public:
  PipelineHazardController(ushort XLEN, RegisterFile* registerFile, bool isRV32E);

  void enqueue(AbstractInstruction* instruction);
  bool checkForStaleRegister(ushort reg);
  bytes fetchRegisterValue(ushort reg);
  void storeResultAfterExecution(bytes result);
};

#endif
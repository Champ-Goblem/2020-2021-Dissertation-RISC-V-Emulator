#pragma once

#include "../AbstractISA.h"

class TExtension : public AbstractISA {
  public:
  TExtension() {
    this->opcodeSpace = vector<OpcodeSpace>{
      OpcodeSpace{
        .opcode = 127,
        .decodeRoutine = &decode
      }
    };
  }

  private:
  static AbstractInstruction decode(bytes instruction, PipelineHazardController* pipelineController, bool* stall);

  static void execute(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize, PipelineHazardController* pipelineController);

  static void writeback(AbstractInstruction* instruction, RegisterFile* registerFile);
};
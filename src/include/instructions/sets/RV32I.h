#ifndef __RV32I__
#define __RV32I__

#include "AbstractISA.h"
#include "../../hw/RegisterFile.h"
#include "../../units/AbstractBranchPredictor.h"

class RV32I: public AbstractISA {
  public:
  RV32I() {
    this->opcodeSpace = vector<OpcodeSpace>{
      // LUI
      OpcodeSpace{
        .opcode = 55,
        .decodeRoutine = &decodeLUI
      },
      // AUIPC
      OpcodeSpace {
        .opcode = 23,
        .decodeRoutine = &decodeAUIPC
      },
      // JAL
      OpcodeSpace {
        .opcode = 111,
        .decodeRoutine = &decodeJAL
      },
      //JALR
      OpcodeSpace{
        .opcode = 103,
        .decodeRoutine = &decodeJALR
      },
      // BEQ | BNE | BLT | BGE | BLTU | BGEU
      OpcodeSpace{
        .opcode = 99,
        .decodeRoutine = &decodeBranch
      },
      // LB | LH | LW | LBU | LHU
      OpcodeSpace{
        .opcode = 3,
        .decodeRoutine = &decodeLoad
      },
      // SB | SH | SW
      OpcodeSpace{
        .opcode = 35,
        .decodeRoutine = &decodeStore
      },
      // ADDI | SLTI | SLTIU | XORI | ORI | ANDI | SLLI | SRLI | SRAI
      OpcodeSpace{
        .opcode = 19,
        .decodeRoutine = &decodeBitopsImmediate
      },
      // ADD | SUB | SLL | SLT | SLTU | XOR | SRL | SRA | OR | AND
      OpcodeSpace{
        .opcode = 51,
        .decodeRoutine = &decodeBitops
      },
      // FENCE
      OpcodeSpace{
        .opcode = 15,
        .decodeRoutine = &decodeFence
      },
      // ECALL | EBREAK
      OpcodeSpace{
        .opcode = 115,
        .decodeRoutine = &decodeERoutines
      }
    };
  }

  private:
  // Decode routines:
  static AbstractInstruction decodeBranch(bytes instruction, PipelineHazardController* pipelineController, bool* stall);
  static AbstractInstruction decodeLUI(bytes instruction, PipelineHazardController* pipelineController, bool* stall);
  static AbstractInstruction decodeAUIPC(bytes instruction, PipelineHazardController* pipelineController, bool* stall);
  static AbstractInstruction decodeJAL(bytes instruction, PipelineHazardController* pipelineController, bool* stall);
  static AbstractInstruction decodeJALR(bytes instruction, PipelineHazardController* pipelineController, bool* stall);
  static AbstractInstruction decodeLoad(bytes instruction, PipelineHazardController* pipelineController, bool* stall);
  static AbstractInstruction decodeStore(bytes instruction, PipelineHazardController* pipelineController, bool* stall);
  static AbstractInstruction decodeBitopsImmediate(bytes instruction, PipelineHazardController* pipelineController, bool* stall);
  static AbstractInstruction decodeBitops(bytes instruction, PipelineHazardController* pipelineController, bool* stall);
  static AbstractInstruction decodeFence(bytes instruction, PipelineHazardController* pipelineController, bool* stall);
  static AbstractInstruction decodeERoutines(bytes instruction, PipelineHazardController* pipelineController, bool* stall);

  // Execute routines:
  static void executeBranch(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize, PipelineHazardController* pipelineController);
  static void executeLUI(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize, PipelineHazardController* pipelineController);
  static void executeAUIPC(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize, PipelineHazardController* pipelineController);
  static void executeJAL(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize, PipelineHazardController* pipelineController);
  static void executeJALR(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize, PipelineHazardController* pipelineController);
  static void executeLoad(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize, PipelineHazardController* pipelineController);
  static void executeStore(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize, PipelineHazardController* pipelineController);
  static void executeBitopsImmediate(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize, PipelineHazardController* pipelineController);
  static void executeBitops(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize, PipelineHazardController* pipelineController);
  static void executeFence(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize, PipelineHazardController* pipelineController);
  static void executeERoutines(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize, PipelineHazardController* pipelineController);

  // Register writeback routines
  static void writebackLUI(AbstractInstruction* instruction, RegisterFile* registerFile);
  static void writebackAUIPC(AbstractInstruction* instruction, RegisterFile* registerFile);
  static void writebackJAL(AbstractInstruction* instruction, RegisterFile* registerFile);
  static void writebackJALR(AbstractInstruction* instruction, RegisterFile* registerFile);
  static void writebackLoad(AbstractInstruction* instruction, RegisterFile* registerFile);
  static void writebackBitopsImmediate(AbstractInstruction* instruction, RegisterFile* registerFile);
  static void writebackBitops(AbstractInstruction* instruction, RegisterFile* registerFile);

  // Memory access routines
  static void memLoad(AbstractInstruction* instruction, Memory* memory, PipelineHazardController* pipelineController);
  static void memStore(AbstractInstruction* instruction, Memory* memory, PipelineHazardController* pipelineController);
};

#endif
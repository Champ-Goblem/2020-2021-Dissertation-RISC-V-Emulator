#ifndef __RV32I__
#define __RV32I__

#include "AbstractISA.h"
#include "../../hw/RegisterFile.h"
#include "../../units/AbstractBranchPredictor.h"

class RV32I: public AbstractISA {
  protected:
  const vector<struct OpcodeSpace> opcodeSpace{
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

  public:
  vector<OpcodeSpace> registerOpcodeSpace() { return opcodeSpace; };

  private:
  // Decode routines:
  static AbstractInstruction decodeBranch(bytes instruction);
  static AbstractInstruction decodeLUI(bytes instruction);
  static AbstractInstruction decodeAUIPC(bytes instruction);
  static AbstractInstruction decodeJAL(bytes instruction);
  static AbstractInstruction decodeJALR(bytes instruction);
  static AbstractInstruction decodeLoad(bytes instruction);
  static AbstractInstruction decodeStore(bytes instruction);
  static AbstractInstruction decodeBitopsImmediate(bytes instruction);
  static AbstractInstruction decodeBitops(bytes instruction);
  static AbstractInstruction decodeFence(bytes instruction);
  static AbstractInstruction decodeERoutines(bytes instruction);

  // Execute routines:
  static void executeBranch(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize);
  static void executeLUI(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize);
  static void executeAUIPC(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize);
  static void executeJAL(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize);
  static void executeJALR(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize);
  static void executeLoad(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize);
  static void executeStore(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize);
  static void executeBitopsImmediate(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize);
  static void executeBitops(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize);
  static void executeFence(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize);
  static void executeERoutines(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize);

  // Register writeback routines
  static void writebackLUI(AbstractInstruction* instruction, RegisterFile* registerFile);
  static void writebackAUIPC(AbstractInstruction* instruction, RegisterFile* registerFile);
  static void writebackJAL(AbstractInstruction* instruction, RegisterFile* registerFile);
  static void writebackJALR(AbstractInstruction* instruction, RegisterFile* registerFile);
  static void writebackLoad(AbstractInstruction* instruction, RegisterFile* registerFile);
  static void writebackBitopsImmediate(AbstractInstruction* instruction, RegisterFile* registerFile);
  static void writebackBitops(AbstractInstruction* instruction, RegisterFile* registerFile);

  // Memory access routines
  static void memLoad(AbstractInstruction* instruction, Memory* memory);
  static void memStore(AbstractInstruction* instruction, Memory* memory);
};

#endif
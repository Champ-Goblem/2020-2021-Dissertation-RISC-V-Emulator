#ifndef __RV32I__
#define __RV32I__

#include "AbstractISA.h"

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
  static void executeBranch(AbstractInstruction instruction);
  static void executeLUI(AbstractInstruction instruction);
  static void executeAUIPC(AbstractInstruction instruction);
  static void executeJAL(AbstractInstruction instruction);
  static void executeJALR(AbstractInstruction instruction);
  static void executeLoad(AbstractInstruction instruction);
  static void executeStore(AbstractInstruction instruction);
  static void executeBitopsImmediate(AbstractInstruction instruction);
  static void executeBitops(AbstractInstruction instruction);
  static void executeFence(AbstractInstruction instruction);
  static void executeERoutines(AbstractInstruction instruction);
};

#endif
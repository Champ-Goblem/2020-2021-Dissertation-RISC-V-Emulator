#include "../../include/instructions/sets/RV32I.h"

AbstractInstruction RV32I::decodeLUI(bytes instruction) {
  // U-Type
  UTypeInstruction ins = UTypeInstruction();
  ins.decode(instruction);
  ins.execute = nullptr;
  ins.memoryAccess = nullptr;
  ins.registerWriteback = &writebackLUI;
  return ins;
}

AbstractInstruction RV32I::decodeAUIPC(bytes instruction) {
  // U-Type
  UTypeInstruction ins = UTypeInstruction();
  ins.decode(instruction);
  ins.execute = &executeAUIPC;
  ins.memoryAccess = nullptr;
  ins.registerWriteback = &writebackAUIPC;
  return ins;
}

AbstractInstruction RV32I::decodeJAL(bytes instruction) {
  // J-Type
  JTypeInstruction ins = JTypeInstruction();
  ins.decode(instruction);
  ins.execute = &executeJAL;
  ins.memoryAccess = nullptr;
  ins.registerWriteback = &writebackJAL;
  return ins;
}

AbstractInstruction RV32I::decodeJALR(bytes instruction) {
  // I-Type
  ITypeInstruction ins = ITypeInstruction();
  ins.decode(instruction);
  ins.execute = &executeJALR;
  ins.memoryAccess = nullptr;
  ins.registerWriteback = &writebackJALR;
  return ins;
}

AbstractInstruction RV32I::decodeBranch(bytes instruction) {
  // B-Type
  BTypeInstruction ins = BTypeInstruction();
  ins.decode(instruction);
  ins.execute = &executeBranch;
  ins.memoryAccess = nullptr;
  ins.registerWriteback = &writebackBranch;
  return ins;
}

AbstractInstruction RV32I::decodeLoad(bytes instruction) {
  // I-Type
  ITypeInstruction ins = ITypeInstruction();
  ins.decode(instruction);
  ins.execute = &executeLoad;
  ins.memoryAccess = &memLoad;
  ins.registerWriteback = &writebackLoad;
  return ins;
}

AbstractInstruction RV32I::decodeStore(bytes instruction) {
  // S-Type
  STypeInstruction ins = STypeInstruction();
  ins.decode(instruction);
  ins.memoryAccess = &memStore;
  ins.registerWriteback = nullptr;
  return ins;
}

AbstractInstruction RV32I::decodeBitopsImmediate(bytes instruction) {
  // I-Type
  ITypeInstruction ins = ITypeInstruction();
  ins.decode(instruction);
  ins.execute = &executeBitopsImmediate;
  ins.memoryAccess = nullptr;
  ins.registerWriteback = &writebackBitopsImmediate;
  if (ins.getFunc3() == 1 || ins.getFunc3() == 5) {
    RTypeInstruction ins = RTypeInstruction(ins);
  }
  return ins;
}

AbstractInstruction RV32I::decodeBitops(bytes instruction) {
  // R-Type
  RTypeInstruction ins = RTypeInstruction();
  ins.decode(instruction);
  ins.execute = &executeBitops;
  ins.memoryAccess = nullptr;
  ins.registerWriteback = &writebackBitops;
  return ins;
}

AbstractInstruction RV32I::decodeFence(bytes instruction) {
  // I-Type
  ITypeInstruction ins = ITypeInstruction();
  ins.decode(instruction);
  ins.execute = &executeFence;
  ins.memoryAccess = nullptr;
  ins.registerWriteback = nullptr;
  return ins;
}

AbstractInstruction RV32I::decodeERoutines(bytes instruction) {
  // I-Type
  ITypeInstruction ins = ITypeInstruction();
  ins.decode(instruction);
  ins.execute = &executeERoutines;
  ins.memoryAccess = nullptr;
  ins.registerWriteback = nullptr;
  return ins;
}

void RV32I::executeBranch(AbstractInstruction* instruction) {
  if (instruction->getType() != InstructionType::B) {
    throw UndefinedInstructionException(instruction, "Instruction not B-Type as expected");
  }

  bytes result;
  if (instruction->isSignedImmediate()) {
    bytes imm = bytes(instruction->getImm());
    result = bytesAdditionSigned(instruction->getPC(), imm);
  }
  switch (instruction->getFunc3()) {
    case 0:
      if (instruction->getRs1Val() == instruction->getRs2Val()) {
        instruction->setResult(result);
      }
      break;
    case 1:
      if (instruction->getRs1Val() != instruction->getRs2Val()) {
        instruction->setResult(result);
      }
      break;
    case 3:
      if (bytesLessThanBytesSigned(instruction->getRs1Val(), instruction->getRs2Val())) {
        instruction->setResult(result);
      }
      break;
    case 4:
      if (bytesGreaterOrequalToSigned(instruction->getRs1Val(), instruction->getRs2Val())) {
        instruction->setResult(result);
      }
      break;
    case 5:
      if (bytesLessThanBytesUnsigned(instruction->getRs1Val(), instruction->getRs2Val())) {
        instruction->setResult(result);
      }
      break;
    case 6:
      if (bytesGreaterOrequalToUnsigned(instruction->getRs1Val(), instruction->getRs2Val())) {
        instruction->setResult(result);
      }
      break;

    default:
      throw UndefinedInstructionException(instruction);
  }
}

void RV32I::executeAUIPC(AbstractInstruction* instruction) {
  if (instruction->getType() != InstructionType::U) {
    throw UndefinedInstructionException(instruction, "Instruction not U-Type as expected");
  }

  instruction->setResult(bytesAdditionSigned(instruction->getPC(), instruction->getImm()));
}

void RV32I::executeJAL(AbstractInstruction* instruction) {
  if (instruction->getType() != InstructionType::J) {
    throw UndefinedInstructionException(instruction, "Instruction not J-Type as expected");
  }

  instruction->setResult(bytesAdditionSigned(instruction->getPC(), instruction->getImm()));
}

void RV32I::executeJALR(AbstractInstruction* instruction) {
  if (instruction->getType() != InstructionType::I) {
    throw UndefinedInstructionException(instruction, "Instruction not I-Type as expected");
  }

  instruction->setResult(bytesAdditionSigned(instruction->getRs1Val(), instruction->getImm()));
}

void RV32I::executeLoad(AbstractInstruction* instruction) {
  if (instruction->getType() != InstructionType::I) {
    throw UndefinedInstructionException(instruction, "Instruction not I-Type as expected");
  }

  instruction->setResult(bytesAdditionSigned(instruction->getRs1Val(), instruction->getImm()));
}

void RV32I::executeStore(AbstractInstruction* instruction) {
  if (instruction->getType() != InstructionType::S) {
    throw UndefinedInstructionException(instruction, "Instruction not S-Type as expected");
  }

  instruction->setResult(bytesAdditionSigned(instruction->getRs1Val(), instruction->getImm()));
}

void RV32I::executeBitopsImmediate(AbstractInstruction* instruction) {
  if (instruction->getType() != InstructionType::I) {
    throw UndefinedInstructionException(instruction, "Instruction not R-Type as expected");
  }

  byte func3 = instruction->getFunc3();
  switch (func3) {
    case 0:
    {
      instruction->setResult(bytesAdditionSigned(instruction->getRs1Val(), instruction->getImm()));
      break;
    }
    case 2:
    {
      bytes isLessThan = bytes{bytesLessThanBytesSigned(instruction->getRs1Val(), instruction->getImm())};
      instruction->setResult(isLessThan);
      break;
    }
    case 3:
    {
      bytes isLessThan = bytes(bytesLessThanBytesUnsigned(instruction->getRs1Val(), instruction->getImm()));
      instruction->setResult(isLessThan);
      break;
    }
    case 4:
    {
      instruction->setResult(bytesXOR(instruction->getRs1Val(), instruction->getImm()));
      break;
    }
    case 6:
    {
      instruction->setResult(bytesOR(instruction->getRs1Val(), instruction->getImm()));
      break;
    }
    case 1:
    {
      instruction->setResult(bytesLogicalLeftShift(instruction->getRs1Val(), instruction->getImm()[0] & 31));
      break;
    }
    case 5:
    {
      if (getContrainedBits(instruction->getImm(), 5, 11)[0] == 0) {
        // SRLI
        instruction->setResult(bytesLogicalRightShift(instruction->getRs2Val(), instruction->getImm()[0] & 31));
      } else if (getContrainedBits(instruction->getImm(), 5, 11)[0] == 64) {
        // SRAI
        instruction->setResult(bytesArithmeticRightShift(instruction->getRs1Val(), instruction->getImm()[0] & 31));
      } else {
        throw UndefinedInstructionException(instruction, "Failed to decode, func7 undefined");
      }
      break;
    }

    default:
      throw UndefinedInstructionException(instruction, "Failed to decode, func3 undefined");
  }
}

void RV32I::executeBitops(AbstractInstruction* instruction) {
  if (instruction->getType() != InstructionType::R) {
    throw UndefinedInstructionException(instruction, "Instruction not I-Type as expected");
  }

  byte func3 = instruction->getFunc3();
  switch (func3) {
    case 0:
    {
      if (instruction->getFunc7() == 0) {
        // ADD
        instruction->setResult(bytesAdditionUnsigned(instruction->getRs1Val(), instruction->getRs2Val()));
      } else if (instruction->getFunc7() == 64) {
        // SUB
        instruction->setResult(bytesSubtractionUnsigned(instruction->getRs1Val(), instruction->getRs2Val()));
      } else {
        throw UndefinedInstructionException(instruction, "Fail to decode, func7 undefined");
      }
      break;
    }
    case 1:
    {
      instruction->setResult(bytesLogicalLeftShift(instruction->getRs1Val(), instruction->getRs2Val()));
      break;
    }
    case 2:
    {
      bytes isLessThan = bytes{bytesLessThanBytesSigned(instruction->getRs1Val(), instruction->getRs2Val())};
      instruction->setResult(isLessThan);
      break;
    }
    case 3:
    {
      bytes isLessThan = bytes{bytesLessThanBytesUnsigned(instruction->getRs1Val(), instruction->getRs2Val())};
      instruction->setResult(isLessThan);
      break;
    }
    case 4:
    {
      instruction->setResult(bytesXOR(instruction->getRs1Val(), instruction->getRs2Val()));
      break;
    }
    case 5:
    {
      if (instruction->getFunc3() == 0) {
        instruction->setResult(bytesLogicalRightShift(instruction->getRs1Val(), instruction->getRs2Val()));
      } else if (instruction->getFunc3() == 64) {
        instruction->setResult(bytesArithmeticRightShift(instruction->getRs1Val(), instruction->getRs2Val()));
      } else {
        throw UndefinedInstructionException(instruction, "Failed to decode, func7 undefined");
      }
      break;
    }
    case 6:
    {
      instruction->setResult(bytesOR(instruction->getRs1Val(), instruction->getRs2Val()));
      break;
    }
    case 7:
    {
      instruction->setResult(bytesAND(instruction->getRs1Val(), instruction->getRs2Val()));
      break;
    }

    default:
      throw UndefinedInstructionException(instruction, "Failed to decode, func3 undefined");
  }
}

void RV32I::executeFence(AbstractInstruction* instruction) {}
void RV32I::executeERoutines(AbstractInstruction* instruction) {}

void RV32I::writebackLUI(AbstractInstruction* instruction, RegisterFile* registerFile) {
  
}
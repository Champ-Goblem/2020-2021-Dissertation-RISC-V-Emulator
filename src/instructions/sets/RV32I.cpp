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
  ins.registerWriteback = nullptr;
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

void RV32I::executeBranch(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize) {
  if (instruction->getType() != InstructionType::B) {
    throw UndefinedInstructionException(instruction, "Instruction not B-Type as expected");
  }

  bytes imm = bytes(instruction->getImm());
  bytes branchTaken = bytesAdditionSigned(instruction->getPC(), imm);
  bytes incPC = addByteToBytes(instruction->getPC(), 4);
  
  switch (instruction->getFunc3()) {
    case 0:
      if (instruction->getRs1Val() == instruction->getRs2Val()) {
        instruction->setResult(branchTaken);
      } else {
        instruction->setResult(incPC);
      }
      break;
    case 1:
      if (instruction->getRs1Val() != instruction->getRs2Val()) {
        instruction->setResult(branchTaken);
      } else {
        instruction->setResult(incPC);
      }
      break;
    case 3:
      if (bytesLessThanBytesSigned(instruction->getRs1Val(), instruction->getRs2Val())) {
        instruction->setResult(branchTaken);
      } else {
        instruction->setResult(incPC);
      }
      break;
    case 4:
      if (bytesGreaterOrequalToSigned(instruction->getRs1Val(), instruction->getRs2Val())) {
        instruction->setResult(branchTaken);
      } else {
        instruction->setResult(incPC);
      }
      break;
    case 5:
      if (bytesLessThanBytesUnsigned(instruction->getRs1Val(), instruction->getRs2Val())) {
        instruction->setResult(branchTaken);
      } else {
        instruction->setResult(incPC);
      }
      break;
    case 6:
      if (bytesGreaterOrequalToUnsigned(instruction->getRs1Val(), instruction->getRs2Val())) {
        instruction->setResult(branchTaken);
      } else {
        instruction->setResult(incPC);
      }
      break;

    default:
      throw UndefinedInstructionException(instruction);
  }

  bytes nextPC = instruction->getResult();

  if (nextPC[0] % 4 != 0) {
    throw AddressMisalignedException(instruction, "Exception generated on branch instruction");
  }

  ulong pcVal = getBytesToULong(nextPC);
  if (pcVal >= memorySize) {
    throw AddressOutOfMemoryException(pcVal, 4, memorySize, true);
  }

  if (!branchPredictor->checkPrediction(instruction->getPC(), nextPC)) {
    throw FailedBranchPredictionException(instruction, "Prediction wrong for branch instruction");
  }
}

void RV32I::executeAUIPC(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize) {
  if (instruction->getType() != InstructionType::U) {
    throw UndefinedInstructionException(instruction, "Instruction not U-Type as expected");
  }

  instruction->setResult(bytesAdditionSigned(instruction->getPC(), instruction->getImm()));
}

void RV32I::executeJAL(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize) {
  if (instruction->getType() != InstructionType::J) {
    throw UndefinedInstructionException(instruction, "Instruction not J-Type as expected");
  }

  bytes nextPC = bytesAdditionSigned(instruction->getPC(), instruction->getImm());
  instruction->setResult(nextPC);

  if (nextPC[0] % 4 != 0) {
    throw AddressMisalignedException(instruction, "Exception generated on JAL instruction");
  }

  ulong pcVal = getBytesToULong(nextPC);
  if (pcVal >= memorySize) {
    throw AddressOutOfMemoryException(pcVal, 4, memorySize, true);
  }

  // Raise exception for handling thread to deal with
  if (!branchPredictor->checkPrediction(instruction->getPC(), nextPC)) {
    throw FailedBranchPredictionException(instruction, "JAL");
  }
}

void RV32I::executeJALR(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize) {
  if (instruction->getType() != InstructionType::I) {
    throw UndefinedInstructionException(instruction, "Instruction not I-Type as expected");
  }

  bytes nextPC = bytesAdditionSigned(instruction->getRs1Val(), instruction->getImm());
  instruction->setResult(nextPC);

  if (nextPC[0] % 4 != 0) {
    throw AddressMisalignedException(instruction, "Exception generated on JALR instruction");
  }

  ulong pcVal = getBytesToULong(nextPC);
  if (pcVal >= memorySize) {
    throw AddressOutOfMemoryException(pcVal, 4, memorySize, true);
  }

  // Raise exception for handling thread to deal with
  if (!branchPredictor->checkPrediction(instruction->getPC(), nextPC)) {
    throw FailedBranchPredictionException(instruction, "JALR");
  }
}

void RV32I::executeLoad(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize) {
  if (instruction->getType() != InstructionType::I) {
    throw UndefinedInstructionException(instruction, "Instruction not I-Type as expected");
  }

  bytes pointer = bytesAdditionSigned(instruction->getRs1Val(), instruction->getImm());
  instruction->setResult(pointer);

  ulong pcVal = getBytesToULong(pointer);
  if (pcVal >= memorySize) {
    throw AddressOutOfMemoryException(pcVal, 4, memorySize, true);
  }
}

void RV32I::executeStore(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize) {
  if (instruction->getType() != InstructionType::S) {
    throw UndefinedInstructionException(instruction, "Instruction not S-Type as expected");
  }

  bytes pointer = bytesAdditionSigned(instruction->getRs1Val(), instruction->getImm());
  instruction->setResult(pointer);

  ulong pcVal = getBytesToULong(pointer);
  if (pcVal >= memorySize) {
    throw AddressOutOfMemoryException(pcVal, 4, memorySize, true);
  }
}

void RV32I::executeBitopsImmediate(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize) {
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

void RV32I::executeBitops(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize) {
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

void RV32I::executeFence(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize) {}
void RV32I::executeERoutines(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize) {}

void RV32I::writebackLUI(AbstractInstruction* instruction, RegisterFile* registerFile) {
  registerFile->write(instruction->getRD(), nextPC);
}

void RV32I::writebackAUIPC(AbstractInstruction* instruction, RegisterFile* registerFile) {
  registerFile->write(instruction->getRD(), nextPC);
}

void RV32I::writebackJAL(AbstractInstruction* instruction, RegisterFile* registerFile) {
  registerFile->write(instruction->getRD(), addByteToBytes(instruction->getPC(), 4));
  
  // Setup PC for when the pipeline is reset
  // TODO: Do we need to write PC
  registerFile->writePC(nextPC);
}

void RV32I::writebackJALR(AbstractInstruction* instruction, RegisterFile* registerFile) {
  registerFile->write(instruction->getRD(), addByteToBytes(instruction->getPC(), 4));
  
  // Setup PC for when the pipeline is reset
  // TODO: Do we need to write pc
  registerFile->writePC(nextPC);
}

void RV32I::writebackLoad(AbstractInstruction* instruction, RegisterFile* registerFile) {
  // Writeback to RD and check and sign extend if necessary
  if (instruction->getFunc3() == 4 || instruction->getFunc3() == 5) {
    // LBU | LHU
    bytes extendedInstruction = nextPC;
    if (instruction->getXLEN() != extendedInstruction.size()) {
      extendedInstruction.resize(instruction->getXLEN());
    }
    registerFile->write(instruction->getRD(), extendedInstruction);
  } else {
    registerFile->write(instruction->getRD(), copyWithSignExtend(nextPC, instruction->getXLEN()));
  }
}

void RV32I::writebackBitopsImmediate(AbstractInstruction* instruction, RegisterFile* registerFile) {
  registerFile->write(instruction->getRD(), nextPC);
}

void RV32I::writebackBitops(AbstractInstruction* instruction, RegisterFile* registerFile) {
  registerFile->write(instruction->getRD(), nextPC);
}

void RV32I::memLoad(AbstractInstruction* instruction, Memory* memory) {
  bytes result;
  switch (instruction->getFunc3()) {
    case 0:
      {
        result = bytes{memory->readByte(getBytesToULong(nextPC))};
        break;
      }
    case 1:
    {
      result = memory->readHWord(getBytesToULong(nextPC));
      break;
    }
    case 2:
    {
      result = memory->readWord(getBytesToULong(nextPC));
      break;
    }
    case 4:
    {
      result = bytes{memory->readByte(getBytesToULong(nextPC))};
      break;
    }
    case 5:
    {
      result = memory->readHWord(getBytesToULong(nextPC));
      break;
    }

    default:
    throw UndefinedInstructionException(instruction, "Func3 undefined during memory access");
  }

  instruction->setResult(result);
}

void RV32I::memStore(AbstractInstruction* instruction, Memory* memory) {
  switch(instruction->getFunc3()) {
    case 0:
    {
      memory->writeByte(getBytesToULong(nextPC), instruction->getRs2Val()[0]);
      break;
    }
    case 1:
    {
      memory->writeHWord(getBytesToULong(nextPC), instruction->getRs2Val());
      break;
    }
    case 2:
    {
      memory->writeWord(getBytesToULong(nextPC), instruction->getRs2Val());
      break;
    }
  }
}
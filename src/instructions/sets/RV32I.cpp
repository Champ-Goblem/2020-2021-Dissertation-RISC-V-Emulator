#include "../../include/instructions/sets/RV32I.h"
#include "../../include/exceptions.h"
#include "../../include/instructions/AbstractInstruction.h"
#include "../../include/instructions/BType.h"
#include "../../include/instructions/IType.h"
#include "../../include/instructions/JType.h"
#include "../../include/instructions/RType.h"
#include "../../include/instructions/SType.h"
#include "../../include/instructions/UType.h"
#include "../../include/units/PipelineHazardController.h"
#include "../../include/hw/Memory.h"

// TODO: Decode needs to fetch register values

AbstractInstruction RV32I::decodeLUI(bytes instruction, PipelineHazardController* pipelineController) {
  // U-Type
  UTypeInstruction ins = UTypeInstruction(pipelineController->getXLEN());
  ins.decode(instruction);
  ins.execute = nullptr;
  ins.memoryAccess = nullptr;
  ins.registerWriteback = &writebackLUI;
  pipelineController->enqueue((AbstractInstruction*)&ins);
  return ins;
}

AbstractInstruction RV32I::decodeAUIPC(bytes instruction, PipelineHazardController* pipelineController) {
  // U-Type
  UTypeInstruction ins = UTypeInstruction(pipelineController->getXLEN());
  ins.decode(instruction);
  ins.execute = &executeAUIPC;
  ins.memoryAccess = nullptr;
  ins.registerWriteback = &writebackAUIPC;
  pipelineController->enqueue(&ins);
  return ins;
}

AbstractInstruction RV32I::decodeJAL(bytes instruction, PipelineHazardController* pipelineController) {
  // J-Type
  JTypeInstruction ins = JTypeInstruction(pipelineController->getXLEN());
  ins.decode(instruction);
  ins.execute = &executeJAL;
  ins.memoryAccess = nullptr;
  ins.registerWriteback = &writebackJAL;
  pipelineController->enqueue(&ins);
  return ins;
}

AbstractInstruction RV32I::decodeJALR(bytes instruction, PipelineHazardController* pipelineController) {
  // I-Type
  ITypeInstruction ins = ITypeInstruction(pipelineController->getXLEN());
  ins.decode(instruction);
  ins.execute = &executeJALR;
  ins.memoryAccess = nullptr;
  ins.registerWriteback = &writebackJALR;
  pipelineController->enqueue(&ins);
  if (pipelineController->checkForStaleRegister(ins.getRS1())) {
    return NOP;
  } else {
    ins.setRs1Val(pipelineController->fetchRegisterValue(ins.getRS1()));
  }
  return ins;
}

AbstractInstruction RV32I::decodeBranch(bytes instruction, PipelineHazardController* pipelineController) {
  // B-Type
  BTypeInstruction ins = BTypeInstruction(pipelineController->getXLEN());
  ins.decode(instruction);
  ins.execute = &executeBranch;
  ins.memoryAccess = nullptr;
  ins.registerWriteback = nullptr;
  pipelineController->enqueue(&ins);
  if (pipelineController->checkForStaleRegister(ins.getRS1()) || pipelineController->checkForStaleRegister(ins.getRS2())) {
    return NOP;
  } else {
    ins.setRs1Val(pipelineController->fetchRegisterValue(ins.getRS1()));
    ins.setRs2Val(pipelineController->fetchRegisterValue(ins.getRS2()));
  }
  return ins;
}

AbstractInstruction RV32I::decodeLoad(bytes instruction, PipelineHazardController* pipelineController) {
  // I-Type
  ITypeInstruction ins = ITypeInstruction(pipelineController->getXLEN());
  ins.decode(instruction);
  ins.execute = &executeLoad;
  ins.memoryAccess = &memLoad;
  ins.registerWriteback = &writebackLoad;
  pipelineController->enqueue(&ins);
  if (pipelineController->checkForStaleRegister(ins.getRS1())) {
    return NOP;
  } else {
    ins.setRs1Val(pipelineController->fetchRegisterValue(ins.getRS1()));
  }
  return ins;
}

AbstractInstruction RV32I::decodeStore(bytes instruction, PipelineHazardController* pipelineController) {
  // S-Type
  STypeInstruction ins = STypeInstruction(pipelineController->getXLEN());
  ins.decode(instruction);
  ins.execute = &executeStore;
  ins.memoryAccess = &memStore;
  ins.registerWriteback = nullptr;
  pipelineController->enqueue(&ins);
  if (pipelineController->checkForStaleRegister(ins.getRS1()) || pipelineController->checkForStaleRegister(ins.getRS2())) {
    return NOP;
  } else {
    ins.setRs1Val(pipelineController->fetchRegisterValue(ins.getRS1()));
    ins.setRs2Val(pipelineController->fetchRegisterValue(ins.getRS2()));
  }
  return ins;
}

AbstractInstruction RV32I::decodeBitopsImmediate(bytes instruction, PipelineHazardController* pipelineController) {
  // I-Type
  ITypeInstruction ins = ITypeInstruction(pipelineController->getXLEN());
  ins.decode(instruction);
  ins.execute = &executeBitopsImmediate;
  ins.memoryAccess = nullptr;
  ins.registerWriteback = &writebackBitopsImmediate;
  if (ins.getFunc3() == 1 || ins.getFunc3() == 5) {
    RTypeInstruction ins = RTypeInstruction(ins);
  }
  pipelineController->enqueue(&ins);
  if (pipelineController->checkForStaleRegister(ins.getRS1())) {
    return NOP;
  } else {
    ins.setRs1Val(pipelineController->fetchRegisterValue(ins.getRS1()));
  }
  return ins;
}

AbstractInstruction RV32I::decodeBitops(bytes instruction, PipelineHazardController* pipelineController) {
  // R-Type
  RTypeInstruction ins = RTypeInstruction(pipelineController->getXLEN());
  ins.decode(instruction);
  ins.execute = &executeBitops;
  ins.memoryAccess = nullptr;
  ins.registerWriteback = &writebackBitops;
  pipelineController->enqueue(&ins);
  if (pipelineController->checkForStaleRegister(ins.getRS1()) || pipelineController->checkForStaleRegister(ins.getRS2())) {
    return NOP;
  } else {
    ins.setRs1Val(pipelineController->fetchRegisterValue(ins.getRS1()));
    ins.setRs2Val(pipelineController->fetchRegisterValue(ins.getRS2()));
  }
  return ins;
}

AbstractInstruction RV32I::decodeFence(bytes instruction, PipelineHazardController* pipelineController) {
  // I-Type
  ITypeInstruction ins = ITypeInstruction(pipelineController->getXLEN());
  ins.decode(instruction);
  ins.execute = &executeFence;
  ins.memoryAccess = nullptr;
  ins.registerWriteback = nullptr;
  pipelineController->enqueue(&ins);
  if (pipelineController->checkForStaleRegister(ins.getRS1())) {
    return NOP;
  } else {
    ins.setRs1Val(pipelineController->fetchRegisterValue(ins.getRS1()));
  }
  return ins;
}

AbstractInstruction RV32I::decodeERoutines(bytes instruction, PipelineHazardController* pipelineController) {
  // I-Type
  ITypeInstruction ins = ITypeInstruction(pipelineController->getXLEN());
  ins.decode(instruction);
  ins.execute = &executeERoutines;
  ins.memoryAccess = nullptr;
  ins.registerWriteback = nullptr;
  pipelineController->enqueue(&ins);
  return ins;
}

void RV32I::executeBranch(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize, PipelineHazardController* pipelineController) {
  if (instruction->getType() != InstructionType::B) {
    throw UndefinedInstructionException(instruction, "Instruction not B-Type as expected");
  }

  bytes imm = bytes(instruction->getImm());
  bytes branchTaken = bytesAddSignedToPC(instruction->getPC(), imm);
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
    case 4:
      if (bytesLessThanBytesSigned(instruction->getRs1Val(), instruction->getRs2Val())) {
        instruction->setResult(branchTaken);
      } else {
        instruction->setResult(incPC);
      }
      break;
    case 5:
      if (bytesGreaterOrequalToSigned(instruction->getRs1Val(), instruction->getRs2Val())) {
        instruction->setResult(branchTaken);
      } else {
        instruction->setResult(incPC);
      }
      break;
    case 6:
      if (bytesLessThanBytesUnsigned(instruction->getRs1Val(), instruction->getRs2Val())) {
        instruction->setResult(branchTaken);
      } else {
        instruction->setResult(incPC);
      }
      break;
    case 7:
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

void RV32I::executeAUIPC(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize, PipelineHazardController* pipelineController) {
  if (instruction->getType() != InstructionType::U) {
    throw UndefinedInstructionException(instruction, "Instruction not U-Type as expected");
  }

  instruction->setResult(bytesAddSignedToPC(instruction->getPC(), instruction->getImm()));
}

void RV32I::executeJAL(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize, PipelineHazardController* pipelineController) {
  if (instruction->getType() != InstructionType::J) {
    throw UndefinedInstructionException(instruction, "Instruction not J-Type as expected");
  }

  bytes jumpPC = bytesAddSignedToPC(instruction->getPC(), instruction->getImm());
  bytes nextPC = addByteToBytes(instruction->getPC(), 4);
  instruction->setResult(nextPC);

  if (jumpPC[0] % 4 != 0) {
    throw AddressMisalignedException(instruction, "Exception generated on JAL instruction");
  }

  ulong pcVal = getBytesToULong(jumpPC);
  if (pcVal >= memorySize) {
    throw AddressOutOfMemoryException(pcVal, 4, memorySize, true);
  }

  // Raise exception for handling thread to deal with
  if (!branchPredictor->checkPrediction(instruction->getPC(), jumpPC)) {
    throw FailedBranchPredictionException(instruction, "JAL");
  }
}

void RV32I::executeJALR(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize, PipelineHazardController* pipelineController) {
  if (instruction->getType() != InstructionType::I) {
    throw UndefinedInstructionException(instruction, "Instruction not I-Type as expected");
  }

  bytes jumpPC = bytesAdditionSigned(instruction->getRs1Val(), instruction->getImm());
  bytes nextPC = addByteToBytes(instruction->getPC(), 4);
  instruction->setResult(nextPC);

  if (jumpPC[0] % 4 != 0) {
    throw AddressMisalignedException(instruction, "Exception generated on JALR instruction");
  }

  ulong pcVal = getBytesToULong(jumpPC);
  if (pcVal >= memorySize) {
    throw AddressOutOfMemoryException(pcVal, 4, memorySize, true);
  }

  // Raise exception for handling thread to deal with
  if (!branchPredictor->checkPrediction(instruction->getPC(), jumpPC)) {
    throw FailedBranchPredictionException(instruction, "JALR");
  }
}

void RV32I::executeLoad(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize, PipelineHazardController* pipelineController) {
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

void RV32I::executeStore(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize, PipelineHazardController* pipelineController) {
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

void RV32I::executeBitopsImmediate(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize, PipelineHazardController* pipelineController) {
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

void RV32I::executeBitops(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize, PipelineHazardController* pipelineController) {
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

void RV32I::executeFence(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize, PipelineHazardController* pipelineController) {}
void RV32I::executeERoutines(AbstractInstruction* instruction, AbstractBranchPredictor* branchPredictor, ulong memorySize, PipelineHazardController* pipelineController) {}

void RV32I::writebackLUI(AbstractInstruction* instruction, RegisterFile* registerFile) {
  registerFile->write(instruction->getRD(), instruction->getImm());
}

void RV32I::writebackAUIPC(AbstractInstruction* instruction, RegisterFile* registerFile) {
  registerFile->write(instruction->getRD(), instruction->getResult());
}

void RV32I::writebackJAL(AbstractInstruction* instruction, RegisterFile* registerFile) {
  registerFile->write(instruction->getRD(), instruction->getResult());
}

void RV32I::writebackJALR(AbstractInstruction* instruction, RegisterFile* registerFile) {
  registerFile->write(instruction->getRD(), instruction->getResult());
}

void RV32I::writebackLoad(AbstractInstruction* instruction, RegisterFile* registerFile) {
  // Writeback to RD and check and sign extend if necessary
  if (instruction->getFunc3() == 4 || instruction->getFunc3() == 5) {
    // LBU | LHU
    bytes extendedResult = instruction->getResult();
    if (instruction->getXLEN() != extendedResult.size()) {
      extendedResult.resize(instruction->getXLEN());
    }
    registerFile->write(instruction->getRD(), extendedResult);
  } else {
    registerFile->write(instruction->getRD(), copyWithSignExtend(instruction->getResult(), instruction->getXLEN()));
  }
}

void RV32I::writebackBitopsImmediate(AbstractInstruction* instruction, RegisterFile* registerFile) {
  registerFile->write(instruction->getRD(), instruction->getResult());
}

void RV32I::writebackBitops(AbstractInstruction* instruction, RegisterFile* registerFile) {
  registerFile->write(instruction->getRD(), instruction->getResult());
}

void RV32I::memLoad(AbstractInstruction* instruction, Memory* memory) {
  bytes result;
  switch (instruction->getFunc3()) {
    case 0:
      {
        result = bytes{memory->readByte(getBytesToULong(instruction->getResult()))};
        break;
      }
    case 1:
    {
      result = memory->readHWord(getBytesToULong(instruction->getResult()));
      break;
    }
    case 2:
    {
      result = memory->readWord(getBytesToULong(instruction->getResult()));
      break;
    }
    case 4:
    {
      result = bytes{memory->readByte(getBytesToULong(instruction->getResult()))};
      break;
    }
    case 5:
    {
      result = memory->readHWord(getBytesToULong(instruction->getResult()));
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
      memory->writeByte(getBytesToULong(instruction->getResult()), instruction->getRs2Val()[0]);
      break;
    }
    case 1:
    {
      bytes value = instruction->getRs2Val();
      memory->writeHWord(getBytesToULong(instruction->getResult()), bytes{value[0], value[1]});
      break;
    }
    case 2:
    {
      memory->writeWord(getBytesToULong(instruction->getResult()), instruction->getRs2Val());
      break;
    }
  }
}
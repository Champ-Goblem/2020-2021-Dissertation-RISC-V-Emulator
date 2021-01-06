#include "../../include/instructions/sets/RV32I.h"

AbstractInstruction RV32I::decodeLUI(bytes instruction) {
  // U-Type
  UTypeInstruction ins = UTypeInstruction();
  ins.decode(instruction);
  ins.execute = &executeLUI;
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
  bytes result;
  if (instruction->isSignedImmediate()) {
    bytes imm = bytes(instruction->getImm());
    result = bytesAddition(instruction->getPC(), imm);
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
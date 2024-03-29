#include "../../../framework/cxxtest-4.4/cxxtest/TestSuite.h"
#include "../../../../src/include/instructions/sets/RV32I.h"
#include "../../../../src/include/units/SimpleBranchPredictor.h"
#include "../../../../src/include/instructions/AbstractInstruction.h"
#include "../../../../src/include/units/PipelineHazardController.h"
#include "../../../../src/include/hw/Memory.h"
#include "../../../../src/include/exceptions.h"
#include "../../../../src/include/instructions/RType.h"
class RV32ITests : public CxxTest::TestSuite
{
  RV32I base;
  vector<OpcodeSpace> OpS = base.registerOpcodeSpace();
  RTypeInstruction __NOP = RTypeInstruction(0, 51, 0, 0, 0, 0, 0);

  public:
  void testLUI(void) {
    // U-Type
    // imm: 10011010111101010000
    // 1110110 10000 10011010111101010000
    // 1110 1101 0000 1001 1010 1111 0101 0000
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 55);
    RegisterFile rf(4, false);
    PipelineHazardController phc(4, &rf, false);
    bool stall;
    AbstractInstruction inst = decode(bytes{183, 144, 245, 10}, &phc, &stall);
    TS_ASSERT(inst.getType() == InstructionType::U);
    inst.registerWriteback(&inst, &rf);
    // res: 0000 0000 0000 1001 1010 1111 0101 000
    bytes result{0, 144, 245, 10};
    TS_ASSERT(rf.get(1) == result);
  }

  void testPosAUIPC(void) {
    // U-Type
    // imm: 00011000000111111100
    // 1110100 10000 00011000000111111100
    // 1110 1001 0000 0001 1000 0001 1111 1100
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 23);
    RegisterFile rf(4, false);
    PipelineHazardController phc(4, &rf, false);
    bool stall;
    AbstractInstruction inst = decode(bytes{151, 128, 129, 63}, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(bytes{0,0,0,0});
    TS_ASSERT(inst.getType() == InstructionType::U);
    inst.execute(&inst, nullptr, 0, &phc);
    inst.registerWriteback(&inst, &rf);
    // res: 0000 0000 0000 0001 1000 0001 1111 1100
    bytes result{0, 128, 129, 63};
    TS_ASSERT(rf.get(1) == result);
  }

  void testNegAUIPC(void) {
    // U-Type
    // imm: 00011000000111111111
    // 1110100 10000 00011000000111111111
    // 1110 1001 0000 0001 1000 0001 1111 1111
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 23);
    RegisterFile rf(4, false);
    PipelineHazardController phc(4, &rf, false);
    bool stall;
    AbstractInstruction inst = decode(bytes{151, 128, 129, 255}, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(bytes{1,12,0,2});
    TS_ASSERT(inst.getType() == InstructionType::U);
    inst.execute(&inst, nullptr, 0, &phc);
    inst.registerWriteback(&inst, &rf);
    bytes result{1, 140, 129, 1};
    TS_ASSERT(rf.get(1) == result);
  }

  void testPosJAL(void) {
    // J-Type
    // imm: 00001000001000000110
    // 1111011 10000 00000011 1 0000100000 0
    // 1111 0111 0000 0000 0011 1000 0100 0000
    Memory m(789350);
    m.writeWord(768, bytes{239, 0, 28, 2});
    RegisterFile rf(4, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, bytes{0, 3, 0, 0});
    PipelineHazardController phc(4, &rf, false);
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 111);
    bool stall;
    AbstractInstruction inst = decode(bytes{239, 0, 28, 2}, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::J);
    inst.execute(&inst, (AbstractBranchPredictor*)&sbp, m.getSize(), &phc);
    inst.registerWriteback(&inst, &rf);
    bytes ret{4, 3, 0, 0};
    TS_ASSERT(rf.get(1) == ret);
  }

  void testNegJAL(void) {
    // J-Type
    // imm: 00001000001011111111 -6112
    // 1111011 10000 01111111 1 0000100000 1
    // 1111 0111 0000 0111 1111 1000 0100 0001
    Memory m(8000);
    bytes instruction{239, 224, 31, 130};
    bytes initialPC{100, 25, 0, 0}; // 6500
    m.writeWord(6500, instruction);
    RegisterFile rf(4, false);
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 111);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::J);
    inst.execute(&inst, (AbstractBranchPredictor*)&sbp, m.getSize(), &phc);
    inst.registerWriteback(&inst, &rf);
    bytes ret{104, 25, 0, 0};
    TS_ASSERT(rf.get(1) == ret);
  }

  void testPosJALR(void) {
    // I-Type
    // imm: 011110100010
    // 1110011 10000 000 01000 011110100010
    // 1110 0111 0000 0000 1000 0111 1010 0010
    bytes instruction{231, 0, 225, 69};
    bytes initialPC{200, 0, 0, 0};
    Memory m(3000);
    m.writeWord(200, instruction);
    RegisterFile rf(4, false);
    rf.write(2, bytes{254, 0, 0, 0});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 103);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::I);
    inst.execute(&inst, (AbstractBranchPredictor*)&sbp, m.getSize(), &phc);
    inst.registerWriteback(&inst, &rf);
    bytes ret{204, 0, 0, 0};
    TS_ASSERT(rf.get(1) == ret);
  }

  void testNegJALR(void) {
    // I-Type
    // imm: 011110101011 -674
    // 1110011 10000 000 01000 011110101011
    // 1110 0111 0000 0000 1000 0111 1010 1011
    bytes instruction{231, 0, 225, 213};
    bytes initialPC{200, 0, 0, 0};
    Memory m(3000);
    m.writeWord(200, instruction);
    RegisterFile rf(4, false);
    rf.write(2, bytes{162, 2, 0, 0});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 103);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::I);
    inst.execute(&inst, (AbstractBranchPredictor*)&sbp, m.getSize(), &phc);
    inst.registerWriteback(&inst, &rf);
    bytes ret{204, 0, 0, 0};
    TS_ASSERT(rf.get(1) == ret);
  }

  void testPosBEQCorrect(void) {
    // B-Type
    // imm: 000100000000
    // 1100011 0 0001 000 00000 00000 000000 0
    // 1100 0110 0001 0000 0000 0000 0000 0000
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 99);
    bytes instruction{99, 8, 0, 0};
    bytes initialPC{8,0,0,0};
    Memory m(3000);
    m.writeWord(8, instruction);
    RegisterFile rf(4, false);
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::B);
    TS_ASSERT_THROWS(inst.execute(&inst, &sbp, 200, &phc), FailedBranchPredictionException);
    bytes result{24, 0, 0, 0};
    TS_ASSERT(inst.getResult() == result);
  }

  void testNegBEQCorrect(void) {
    // B-Type
    // imm: 0001111111111
    // 1100011 1 0001 000 00000 00000 111111 1
    // 1100 0111 0001 0000 0000 0000 0111 1111
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 99);
    bytes instruction{227, 8, 0, 254};
    bytes initialPC{32,0,0,0};
    Memory m(3000);
    m.writeWord(32, instruction);
    RegisterFile rf(4, false);
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::B);
    inst.execute(&inst, &sbp, 200, &phc);
    bytes result{16, 0, 0, 0};
    TS_ASSERT(inst.getResult() == result);
  }

  void testPosBEQIncorrect(void) {
    // B-Type
    // imm: 0010000000000
    // 1100011 0 0100 000 00000 10000 000000 0
    // 1100 0110 0100 0000 0000 1000 0000 0000
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 99);
    bytes instruction{99, 2, 16, 0};
    bytes initialPC{32,0,0,0};
    Memory m(3000);
    m.writeWord(32, instruction);
    RegisterFile rf(4, false);
    rf.write(1, bytes{1, 0, 0, 0});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::B);
    inst.execute(&inst, &sbp, 200, &phc);
    bytes result{36, 0, 0, 0};
    TS_ASSERT(inst.getResult() == result);
  }

  void testNegBEQIncorrect(void) {
    // B-Type
    // imm: 0000111111111
    // 1100011 1 0001 000 00000 10000 111111 1
    // 1100 0111 0001 0000 0000 1000 0111 1111
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 99);
    bytes instruction{227, 8, 16, 254};
    bytes initialPC{32,0,0,0};
    Memory m(3000);
    m.writeWord(32, instruction);
    RegisterFile rf(4, false);
    rf.write(1, bytes{1, 0, 0, 0});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::B);
    TS_ASSERT_THROWS(inst.execute(&inst, &sbp, 200, &phc), FailedBranchPredictionException);
    bytes result{36, 0, 0, 0};
    TS_ASSERT(inst.getResult() == result);
  }

  void testPosBNECorrect(void) {
    // B-Type
    // imm: 0000100000000
    // 1100011 0 0001 100 00000 10000 000000 0
    // 1100 0110 0001 1000 0000 1000 0000 0000
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 99);
    bytes instruction{99, 24, 16, 0};
    bytes initialPC{32,0,0,0};
    Memory m(3000);
    m.writeWord(32, instruction);
    RegisterFile rf(4, false);
    rf.write(1, bytes{1, 0, 0, 0});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::B);
    TS_ASSERT_THROWS(inst.execute(&inst, &sbp, 200, &phc), FailedBranchPredictionException);
    bytes result{48, 0, 0, 0};
    TS_ASSERT(inst.getResult() == result);
  }

  void testNegBNECorrect(void) {
    // B-Type
    // imm: 0011111111111
    // 1100011 1 0111 100 00000 10000 111111 1
    // 1100 0111 0111 1000 0000 1000 0111 1111
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 99);
    bytes instruction{227, 30, 16, 254};
    bytes initialPC{32,0,0,0};
    Memory m(3000);
    m.writeWord(32, instruction);
    RegisterFile rf(4, false);
    rf.write(1, bytes{1, 0, 0, 0});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::B);
    inst.execute(&inst, &sbp, 200, &phc);
    bytes result{28, 0, 0, 0};
    TS_ASSERT(inst.getResult() == result);
  }

  void testPosBLTCorrect(void) {
    // B-Type
    // imm: 0000001000000
    // 1100011 0 0000 001 00000 10000 010000 0
    // 1100 0110 0000 0010 0000 1000 0010 0000
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 99);
    bytes instruction{99, 64, 16, 4};
    bytes initialPC{32,0,0,0};
    Memory m(3000);
    m.writeWord(32, instruction);
    RegisterFile rf(4, false);
    rf.write(1, bytes{1, 0, 0, 0});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::B);
    TS_ASSERT_THROWS(inst.execute(&inst, &sbp, 200, &phc), FailedBranchPredictionException);
    bytes result{96, 0, 0, 0};
    TS_ASSERT(inst.getResult() == result);
  }

  void testNegBLTCorrect(void) {
    // B-Type
    // imm: 000111111111
    // 1100011 1 0011 001 10000 00000 111111 1
    // 1100 0111 0011 0011 0000 0000 0111 1111
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 99);
    bytes instruction{227, 204, 0, 254};
    bytes initialPC{32,0,0,0};
    Memory m(3000);
    m.writeWord(32, instruction);
    RegisterFile rf(4, false);
    rf.write(1, bytes{255, 255, 255, 255});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::B);
    inst.execute(&inst, &sbp, 200, &phc);
    bytes result{24, 0, 0, 0};
    TS_ASSERT(inst.getResult() == result);
  }

  void testPosBGECorrect(void) {
    // B-Type
    // imm: 000100000000
    // 1100011 0 0010 101 00000 10000 000000 0
    // 1100 0110 0010 1010 0000 1000 0000 0000
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 99);
    bytes instruction{99, 84, 16, 0};
    bytes initialPC{32,0,0,0};
    Memory m(3000);
    m.writeWord(32, instruction);
    RegisterFile rf(4, false);
    rf.write(1, bytes{255, 255, 255, 255});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::B);
    TS_ASSERT_THROWS(inst.execute(&inst, &sbp, 200, &phc), FailedBranchPredictionException);
    bytes result{40, 0, 0, 0};
    TS_ASSERT(inst.getResult() == result);
  }

  void testNegBGECorrect(void) {
    // B-Type
    // imm: 000011111111
    // 1100011 1 0001 101 00000 10000 111111 1
    // 1100 0111 0001 1010 0000 1000 0111 1111
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 99);
    bytes instruction{227, 88, 16, 254};
    bytes initialPC{32,0,0,0};
    Memory m(3000);
    m.writeWord(32, instruction);
    RegisterFile rf(4, false);
    rf.write(1, bytes{0, 0, 0, 129});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::B);
    inst.execute(&inst, &sbp, 200, &phc);
    bytes result{16, 0, 0, 0};
    TS_ASSERT(inst.getResult() == result);
  }

  void testPosBLTUCorrect(void) {
    // B-Type
    // imm: 0010000000000
    // 1100011 0 0100 011 00000 10000 000000 0
    // 1100 0110 0100 0110 0000 1000 0000 0000
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 99);
    bytes instruction{99, 98, 16, 0};
    bytes initialPC{32,0,0,0};
    Memory m(3000);
    m.writeWord(32, instruction);
    RegisterFile rf(4, false);
    rf.write(1, bytes{255, 255, 255, 255});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::B);
    inst.execute(&inst, &sbp, 200, &phc);
    bytes result{36, 0, 0, 0};
    TS_ASSERT(inst.getResult() == result);
  }

  void testNegBLTUCorrect(void) {
    // B-Type
    // imm: 0000000000111
    // 1100011 1 0000 011 00000 10000 000001 1
    // 1100 0111 0000 0110 0000 1000 0000 0011
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 99);
    bytes instruction{227, 96, 16, 192};
    bytes initialPC{32,4,0,0};
    Memory m(3000);
    m.writeWord(1056, instruction);
    RegisterFile rf(4, false);
    rf.write(1, bytes{255, 255, 255, 255});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::B);
    inst.execute(&inst, &sbp, 200, &phc);
    bytes result{32, 0, 0, 0};
    TS_ASSERT(inst.getResult() == result);
  }

  void testPosBGEUCorrect(void) {
    // B-Type
    // imm: 0001000000000
    // 1100011 0 0010 111 10000 00000 000000 0
    // 1100 0110 0010 1111 0000 0000 0000 0000
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 99);
    bytes instruction{99, 244, 0, 0};
    bytes initialPC{32,4,0,0};
    Memory m(3000);
    m.writeWord(1056, instruction);
    RegisterFile rf(4, false);
    rf.write(1, bytes{255, 255, 255, 255});
    PipelineHazardController phc(4, &rf, false);
    bool stall;
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::B);
    TS_ASSERT_THROWS(inst.execute(&inst, &sbp, 3000, &phc), FailedBranchPredictionException);
    bytes result{40, 4, 0, 0};
    TS_ASSERT(inst.getResult() == result);
  }

  void testNegBGEUIncorrect(void) {
    // B-Type
    // imm: 0000001000011
    // 1100011 1 0000 111 00000 10000 010000 1
    // 1100 0111 0000 1110 0000 1000 0010 0001
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 99);
    bytes instruction{227, 112, 16, 132};
    bytes initialPC{224,7,0,0};
    Memory m(3000);
    m.writeWord(2016, instruction);
    RegisterFile rf(4, false);
    rf.write(1, bytes{255, 255, 255, 255});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::B);
    TS_ASSERT_THROWS(inst.execute(&inst, &sbp, 3000, &phc), FailedBranchPredictionException);
    bytes result{228, 7, 0, 0};
    TS_ASSERT(inst.getResult() == result);
  }

  void testLB(void) {
    // I-Type
    // imm: 000001000100
    // 1100000 10000 000 00000 000001000100
    // 1100 0001 0000 0000 0000 0000 0100 0100
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 3);
    bytes instruction{131, 0, 0, 34};
    bytes initialPC{4,0,0,0};
    byte memoryVal = 60;
    Memory m(3000);
    m.writeWord(4, instruction);
    m.writeByte(544, memoryVal);
    RegisterFile rf(4, false);
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::I);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{32, 2, 0, 0};
    TS_ASSERT(inst.getResult() == result);
    phc.bump();
    phc.enqueue(__NOP);
    inst.memoryAccess(&inst, &m, &phc);
    inst.registerWriteback(&inst, &rf);
    TS_ASSERT(rf.get(1)[0] == memoryVal);
  }

  void testLH(void) {
    // I-Type
    // imm: 111111111111
    // 1100000 10000 100 11000 111111111111
    // 1100 0001 0000 1001 1000 1111 1111 1111
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 3);
    bytes instruction{131, 144, 241, 255};
    bytes initialPC{4,0,0,0};
    bytes memoryVal = {2, 183, 255, 255};
    Memory m(3000);
    m.writeWord(4, instruction);
    m.writeHWord(23, bytes{memoryVal[0], memoryVal[1]});
    RegisterFile rf(4, false);
    rf.write(3, bytes{24, 0, 0, 0});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::I);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{23, 0, 0, 0};
    TS_ASSERT(inst.getResult() == result);
    phc.bump();
    phc.enqueue(__NOP);
    inst.memoryAccess(&inst, &m, &phc);
    inst.registerWriteback(&inst, &rf);
    TS_ASSERT(rf.get(1) == memoryVal);
  }

  void testLW(void) {
    // I-Type
    // imm: 000000100000
    // 1100000 10000 010 11000 000000100000
    // 1100 0001 0000 0101 1000 0000 0010 0000
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 3);
    bytes instruction{131, 160, 1, 4};
    bytes initialPC{4,0,0,0};
    bytes memoryVal = {2, 183, 255, 13};
    Memory m(3000);
    m.writeWord(4, instruction);
    m.writeWord(88, memoryVal);
    RegisterFile rf(4, false);
    rf.write(3, bytes{24, 0, 0, 0});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::I);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{88, 0, 0, 0};
    TS_ASSERT(inst.getResult() == result);
    phc.bump();
    phc.enqueue(__NOP);
    inst.memoryAccess(&inst, &m, &phc);
    inst.registerWriteback(&inst, &rf);
    TS_ASSERT(rf.get(1) == memoryVal);
  }

  void testLBU(void) {
    // I-Type
    // imm: 000001000100
    // 1100000 10000 001 00000 000001000100
    // 1100 0001 0000 0010 0000 0000 0100 0100
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 3);
    bytes instruction{131, 64, 0, 34};
    bytes initialPC{4,0,0,0};
    byte memoryVal = 128;
    Memory m(3000);
    m.writeWord(4, instruction);
    m.writeByte(544, memoryVal);
    RegisterFile rf(4, false);
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::I);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{32, 2, 0, 0};
    TS_ASSERT(inst.getResult() == result);
    phc.bump();
    phc.enqueue(__NOP);
    inst.memoryAccess(&inst, &m, &phc);
    inst.registerWriteback(&inst, &rf);
    bytes memVal{memoryVal, 0, 0, 0};
    TS_ASSERT(rf.get(1) == memVal);
  }

  void testLHU(void) {
    // I-Type
    // imm: 111111111111
    // 1100000 10000 101 11000 111111111111
    // 1100 0001 0000 1011 1000 1111 1111 1111
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 3);
    bytes instruction{131, 208, 241, 255};
    bytes initialPC{4,0,0,0};
    bytes memoryVal = {2, 183, 0, 0};
    Memory m(3000);
    m.writeWord(4, instruction);
    m.writeHWord(23, bytes{memoryVal[0], memoryVal[1]});
    RegisterFile rf(4, false);
    rf.write(3, bytes{24, 0, 0, 0});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::I);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{23, 0, 0, 0};
    TS_ASSERT(inst.getResult() == result);
    phc.bump();
    phc.enqueue(__NOP);
    inst.memoryAccess(&inst, &m, &phc);
    inst.registerWriteback(&inst, &rf);
    TS_ASSERT(rf.get(1) == memoryVal);
  }

  void testSB(void) {
    // S-Type
    // imm: 000100000000
    // 1100010 00010 000 10000 01000 0000000
    // 1100 0100 0010 0001 0000 0100 0000 0000
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 35);
    bytes instruction{35, 132, 32, 0};
    bytes initialPC{4,0,0,0};
    bytes memoryVal = {2, 183, 0, 0};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(2, memoryVal);
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::S);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{8, 0, 0, 0};
    TS_ASSERT(inst.getResult() == result);
    inst.memoryAccess(&inst, &m, &phc);
    bytes memVal{2, 0, 0, 0};
    TS_ASSERT(m.readWord(8) == memVal);
  }

  void testSH(void) {
    // S-Type
    // imm: 100111111111
    // 1100010 10011 100 10000 01000 1111111
    // 1100 0101 0011 1001 0000 0100 0111 1111
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 35);
    bytes instruction{163, 156, 32, 254};
    bytes initialPC{4,0,0,0};
    bytes memoryVal = {2, 183, 32, 0};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(1, bytes{100, 0, 0, 0});
    rf.write(2, memoryVal);
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::S);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{93, 0, 0, 0};
    TS_ASSERT(inst.getResult() == result);
    inst.memoryAccess(&inst, &m, &phc);
    bytes memVal{2, 183, 0, 0};
    TS_ASSERT(m.readWord(93) == memVal);
  }

  void testSW(void) {
    // S-Type
    // imm: 0000010000000
    // 1100010 00000 010 10000 01000 1000000
    // 1100 0100 0000 0101 0000 0100 0100 0000
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 35);
    bytes instruction{35, 160, 32, 2};
    bytes initialPC{4,0,0,0};
    bytes memoryVal = {2, 183, 32, 255};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(1, bytes{100, 0, 0, 0});
    rf.write(2, memoryVal);
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::S);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{132, 0, 0, 0};
    TS_ASSERT(inst.getResult() == result);
    inst.memoryAccess(&inst, &m, &phc);
    TS_ASSERT(m.readWord(132) == memoryVal);
  }

  void testADDIPosNeg(void) {
    // I-Type
    // imm: 000001001111 
    // 1100100 10000 000 01000 000001001111
    // 1100 1001 0000 0000 1000 0000 0100 1111
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 19);
    bytes instruction{147, 0, 1, 242};
    bytes initialPC{4,0,0,0};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(2, bytes{32, 14, 0, 0});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::I);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{64, 13, 0, 0};
    TS_ASSERT(inst.getResult() == result);
    inst.registerWriteback(&inst, &rf);
    TS_ASSERT(rf.get(1) == result);
  }

  void testADDINegNeg(void) {
    // I-Type
    // imm: 000001001111  -224
    // 1100100 10000 000 01000 000001001111
    // 1100 1001 0000 0000 1000 0000 0100 1111
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 19);
    bytes instruction{147, 0, 1, 242};
    bytes initialPC{4,0,0,0};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(2, bytes{32, 254, 255, 255}); // -480
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::I);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{64, 253, 255, 255};
    TS_ASSERT(inst.getResult() == result);
    inst.registerWriteback(&inst, &rf);
    TS_ASSERT(rf.get(1) == result);
  }

  void testSLTIPosNeg(void) {
    // I-Type
    // imm: 000010000000  16
    // 1100100 10000 010 01000 000010000000
    // 1100 1001 0000 0100 1000 0000 1000 0000
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 19);
    bytes instruction{147, 32, 1, 1};
    bytes initialPC{4,0,0,0};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(2, bytes{32, 254, 255, 255}); // -480
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::I);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{1, 0, 0, 0};
    TS_ASSERT(inst.getResult() == result);
    inst.registerWriteback(&inst, &rf);
    TS_ASSERT(rf.get(1) == result);
  }

  void testSLTINegNeg(void) {
    // I-Type
    // imm: 000011111111  -16
    // 1100100 10000 010 01000 000011111111
    // 1100 1001 0000 0100 1000 0000 1111 1111
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 19);
    bytes instruction{147, 32, 1, 255};
    bytes initialPC{4,0,0,0};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(2, bytes{32, 254, 255, 255}); // -480
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::I);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{1, 0, 0, 0};
    TS_ASSERT(inst.getResult() == result);
    inst.registerWriteback(&inst, &rf);
    TS_ASSERT(rf.get(1) == result);
  }

  void testSLTIU(void) {
    // I-Type
    // imm: 000010000000  16
    // 1100100 10000 110 01000 000010000000
    // 1100 1001 0000 1100 1000 0000 1000 0000
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 19);
    bytes instruction{147, 48, 1, 1};
    bytes initialPC{4,0,0,0};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(2, bytes{32, 254, 255, 255});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::I);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{0, 0, 0, 0};
    TS_ASSERT(inst.getResult() == result);
    inst.registerWriteback(&inst, &rf);
    TS_ASSERT(rf.get(1) == result);
  }

  void testXORI(void) {
    // I-Type
    // imm: 000100011110
    // 1100100 10000 001 01000 000100011110
    // 1100 1001 0000 0010 1000 0001 0001 1110
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 19);
    bytes instruction{147, 64, 129, 120};
    bytes initialPC{4,0,0,0};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(2, bytes{0, 23, 24, 0});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::I);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{136, 16, 24, 0};
    TS_ASSERT(inst.getResult() == result);
    inst.registerWriteback(&inst, &rf);
    TS_ASSERT(rf.get(1) == result);
  }

  void testORI(void) {
    // I-Type
    // imm: 000100011110
    // 1100100 10000 011 01000 000100011110
    // 1100 1001 0000 0110 1000 0001 0001 1110
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 19);
    bytes instruction{147, 96, 129, 120};
    bytes initialPC{4,0,0,0};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(2, bytes{0, 23, 24, 0});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::I);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{136, 23, 24, 0};
    TS_ASSERT(inst.getResult() == result);
    inst.registerWriteback(&inst, &rf);
    TS_ASSERT(rf.get(1) == result);
  }

  void testANDI(void) {
    // I-Type
    // imm: 000100011110
    // 1100100 10000 111 01000 000100011110
    // 1100 1001 0000 1110 1000 0001 0001 1110
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 19);
    bytes instruction{147, 112, 129, 120};
    bytes initialPC{4,0,0,0};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(2, bytes{0, 23, 24, 0});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::I);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{0, 7, 0, 0};
    TS_ASSERT(inst.getResult() == result);
    inst.registerWriteback(&inst, &rf);
    TS_ASSERT(rf.get(1) == result);
  }

  void testSLLI(void) {
    // I-Type
    // imm: 00100
    // 1100100 10000 100 01000 00100 0000000
    // 1100 1001 0000 1000 1000 0010 0000 0000
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 19);
    bytes instruction{147, 16, 65, 0};
    bytes initialPC{4,0,0,0};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(2, bytes{162, 23, 24, 0});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::I);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{32, 122, 129, 1};
    TS_ASSERT(inst.getResult() == result);
    inst.registerWriteback(&inst, &rf);
    TS_ASSERT(rf.get(1) == result);
  }

  void testSRLI(void) {
    // I-Type
    // imm: 00100
    // 1100100 10000 101 01000 00100 0000000
    // 1100 1001 0000 1010 1000 0010 0000 0000
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 19);
    bytes instruction{147, 80, 65, 0};
    bytes initialPC{4,0,0,0};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(2, bytes{162, 23, 24, 0});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::I);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{122, 129, 1, 0};
    TS_ASSERT(inst.getResult() == result);
    inst.registerWriteback(&inst, &rf);
    TS_ASSERT(rf.get(1) == result);
  }

  void testSRAI(void) {
    // I-Type
    // imm: 00100
    // 1100100 10000 101 01000 00100 0000010
    // 1100 1001 0000 1010 1000 0010 0000 0010
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 19);
    bytes instruction{147, 80, 65, 64};
    bytes initialPC{4,0,0,0};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(2, bytes{162, 23, 24, 128});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::I);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{122, 129, 1, 248};
    TS_ASSERT(inst.getResult() == result);
    inst.registerWriteback(&inst, &rf);
    TS_ASSERT(rf.get(1) == result);
  }

  void testADDPosPos(void) {
    // R-Type
    // 1100110 10000 000 01000 11000 0000000
    // 1100 1101 0000 0000 1000 1100 0000 0000
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 51);
    bytes instruction{179, 0, 49, 0};
    bytes initialPC{4,0,0,0};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(2, bytes{162, 23, 24, 128});
    rf.write(3, bytes{0, 3, 4, 5});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::R);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{162, 26, 28, 133};
    TS_ASSERT(inst.getResult() == result);
    inst.registerWriteback(&inst, &rf);
    TS_ASSERT(rf.get(1) == result);
  }

  void testSUB(void) {
    // R-Type
    // 1100110 10000 000 01000 11000 0000010
    // 1100 1101 0000 0000 1000 1100 0000 0010
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 51);
    bytes instruction{179, 0, 49, 64};
    bytes initialPC{4,0,0,0};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(2, bytes{162, 23, 24, 128});
    rf.write(3, bytes{0, 3, 4, 5});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::R);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{162, 20, 20, 123};
    TS_ASSERT(inst.getResult() == result);
    inst.registerWriteback(&inst, &rf);
    TS_ASSERT(rf.get(1) == result);
  }

  void testSLL(void) {
    // R-Type
    // 1100110 10000 100 01000 11000 0000000
    // 1100 1101 0000 1000 1000 1100 0000 0000
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 51);
    bytes instruction{179, 16, 49, 0};
    bytes initialPC{4,0,0,0};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(2, bytes{162, 23, 24, 128});
    rf.write(3, bytes{0, 3, 4, 5});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::R);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{0, 0, 0, 0};
    TS_ASSERT(inst.getResult() == result);
    inst.registerWriteback(&inst, &rf);
    TS_ASSERT(rf.get(1) == result);
  }

  void testSLT(void) {
    // R-Type
    // 1100110 10000 010 01000 11000 0000000
    // 1100 1101 0000 0100 1000 1100 0000 0000
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 51);
    bytes instruction{179, 32, 49, 0};
    bytes initialPC{4,0,0,0};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(2, bytes{162, 23, 24, 128});
    rf.write(3, bytes{255, 255, 255, 255});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::R);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{1, 0, 0, 0};
    TS_ASSERT(inst.getResult() == result);
    inst.registerWriteback(&inst, &rf);
    TS_ASSERT(rf.get(1) == result);
  }

  void testSLTU(void) {
    // R-Type
    // 1100110 10000 110 01000 11000 0000000
    // 1100 1101 0000 1100 1000 1100 0000 0000
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 51);
    bytes instruction{179, 48, 49, 0};
    bytes initialPC{4,0,0,0};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(2, bytes{162, 23, 24, 0});
    rf.write(3, bytes{255, 255, 255, 255});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::R);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{1, 0, 0, 0};
    TS_ASSERT(inst.getResult() == result);
    inst.registerWriteback(&inst, &rf);
    TS_ASSERT(rf.get(1) == result);
  }

  void testXOR(void) {
    // R-Type
    // 1100110 10000 001 01000 11000 0000000
    // 1100 1101 0000 0010 1000 1100 0000 0000
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 51);
    bytes instruction{179, 64, 49, 0};
    bytes initialPC{4,0,0,0};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(2, bytes{162, 23, 24, 0});
    rf.write(3, bytes{255, 255, 255, 255});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::R);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{93, 232, 231, 255};
    TS_ASSERT(inst.getResult() == result);
    inst.registerWriteback(&inst, &rf);
    TS_ASSERT(rf.get(1) == result);
  }

  void testSRL(void) {
    // R-Type
    // 1100110 10000 101 01000 11000 0000000
    // 1100 1101 0000 1010 1000 1100 0000 0000
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 51);
    bytes instruction{179, 80, 49, 0};
    bytes initialPC{4,0,0,0};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(2, bytes{162, 23, 24, 0});
    rf.write(3, bytes{13, 0, 0, 0});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::R);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{192, 0, 0, 0};
    TS_ASSERT(inst.getResult() == result);
    inst.registerWriteback(&inst, &rf);
    TS_ASSERT(rf.get(1) == result);
  }

  void testSRA(void) {
    // R-Type
    // 1100110 10000 101 01000 11000 0000010
    // 1100 1101 0000 1010 1000 1100 0000 0010
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 51);
    bytes instruction{179, 80, 49, 64};
    bytes initialPC{4,0,0,0};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(2, bytes{162, 23, 24, 128});
    rf.write(3, bytes{13, 0, 0, 0});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::R);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{192, 0, 252, 255};
    TS_ASSERT(inst.getResult() == result);
    inst.registerWriteback(&inst, &rf);
    TS_ASSERT(rf.get(1) == result);
  }

void testOR(void) {
    // R-Type
    // 1100110 10000 011 01000 11000 0000000
    // 1100 1101 0000 0110 1000 1100 0000 0000
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 51);
    bytes instruction{179, 96, 49, 0};
    bytes initialPC{4,0,0,0};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(2, bytes{162, 23, 24, 128});
    rf.write(3, bytes{13, 0, 255, 0});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::R);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{175, 23, 255, 128};
    TS_ASSERT(inst.getResult() == result);
    inst.registerWriteback(&inst, &rf);
    TS_ASSERT(rf.get(1) == result);
  }

void testAND(void) {
    // R-Type
    // 1100110 10000 111 01000 11000 0000000
    // 1100 1101 0000 1110 1000 1100 0000 0000
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 51);
    bytes instruction{179, 112, 49, 0};
    bytes initialPC{4,0,0,0};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(2, bytes{162, 23, 24, 128});
    rf.write(3, bytes{13, 0, 255, 0});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, &stall);
    phc.bump();
    phc.enqueue(inst);
    phc.bump();
    phc.enqueue(NOP(4));
    phc.bump();
    phc.enqueue(NOP(4));
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::R);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{0, 0, 24, 0};
    TS_ASSERT(inst.getResult() == result);
    inst.registerWriteback(&inst, &rf);
    TS_ASSERT(rf.get(1) == result);
  }
};

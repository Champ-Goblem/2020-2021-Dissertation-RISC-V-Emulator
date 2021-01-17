#include "../../../framework/cxxtest-4.4/cxxtest/TestSuite.h"
#include "../../../../src/include/instructions/sets/RV32I.h"
#include "../../../../src/include/units/SimpleBranchPredictor.h"
#include "../../../../src/include/instructions/AbstractInstruction.h"
#include "../../../../src/include/units/PipelineHazardController.h"
#include "../../../../src/include/hw/Memory.h"

class RV32ITests : public CxxTest::TestSuite
{
  RV32I base;
  vector<OpcodeSpace> OpS = base.registerOpcodeSpace();

  public:
  void testLUI(void) {
    // U-Type
    // imm: 10011010111101010000
    // 1110110 10000 10011010111101010000
    // 1110 1101 0000 1001 1010 1111 0101 0000
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 55);
    RegisterFile rf(4, false);
    PipelineHazardController phc(4, &rf, false);
    AbstractInstruction inst = decode(bytes{183, 144, 245, 10}, &phc);
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
    AbstractInstruction inst = decode(bytes{151, 128, 129, 63}, &phc);
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
    AbstractInstruction inst = decode(bytes{151, 128, 129, 255}, &phc);
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
    AbstractInstruction inst = decode(bytes{239, 0, 28, 2}, &phc);
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::J);
    inst.execute(&inst, (AbstractBranchPredictor*)&sbp, m.getSize(), &phc);
    inst.registerWriteback(&inst, &rf);
    // res: 0000 0100 1101 0000 0011 0000 0000 0000
    bytes result{32, 11, 12, 0};
    TS_ASSERT(inst.getResult() == result);
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
    AbstractInstruction inst = decode(instruction, &phc);
    inst.setPC(sbp.getNextPC());
    TS_ASSERT(inst.getType() == InstructionType::J);
    inst.execute(&inst, (AbstractBranchPredictor*)&sbp, m.getSize(), &phc);
    inst.registerWriteback(&inst, &rf);
    // res: 0010 0001 1000 0000 0000 0000 0000 0000
    bytes result{132, 1, 0, 0};
    TS_ASSERT(inst.getResult() == result);
    bytes ret{104, 25, 0, 0};
    TS_ASSERT(rf.get(1) == ret);
  }

  // void testPosJALR(void) {
  //   // I-Type
  //   // imm: 011110100010 1118
  //   // 1110011 10000 000 11000 011110100010
  //   // 1110 0111 0000 0001 1000 0111 1010 0010
  //   bytes instruction{119, 128, 225, 69};
  //   bytes initialPC{200, 0, 0, 0};
  //   Memory m(3000);
  //   m.writeWord(200, instruction);
  //   RegisterFile rf(4, false);
  //   rf.write(2, bytes{255, 255, 0, 0});
  //   SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
  //   DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 103);
  //   AbstractInstruction inst = decode(instruction);
  //   inst.setPC(sbp.getNextPC());
  //   TS_ASSERT(inst.getType() == InstructionType::I);
  //   inst.execute(&inst, (AbstractBranchPredictor*)&sbp, m.getSize());
  //   inst.registerWriteback(&inst, &rf);
  //   // res: 0101 1010 1110 0000 0000 0000 0000 0000
  //   bytes result{90, 7, 0, 0};
  //   TS_ASSERT(inst.getResult() == result);
  //   bytes ret{204, 0, 0, 0};
  //   TS_ASSERT(rf.get(1) == ret);
  // }

  // void testPosBEQCorrect(void) {
  //   // B-Type
  //   // imm: 000100000000
  //   // 1100011 0 0001 000 00000 00000 000000 0
  //   // 1100 0110 0001 0000 0000 0000 0000 0000
  //   DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 99);
  //   AbstractInstruction inst = decode(bytes{99, 8, 0, 0});
  //   inst.setPC(bytes{0,0,0,8});
  //   TS_ASSERT(inst.getType() == InstructionType::B);
  //   inst.execute(&inst, nullptr, 200);
  //   RegisterFile rf(4, false);
  //   inst.registerWriteback(&inst, &rf);
  //   bytes result{0, 0, 0, 8};
  //   TS_ASSERT(rf.get(1) == result);
  // }
};

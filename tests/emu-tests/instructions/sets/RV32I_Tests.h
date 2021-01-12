#include "../../../framework/cxxtest-4.4/cxxtest/TestSuite.h"
#include "../../../../src/include/instructions/sets/RV32I.h"

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
    AbstractInstruction inst = decode(bytes{183, 144, 245, 10});
    TS_ASSERT(inst.getType() == InstructionType::U);
    RegisterFile rf(4, false);
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
    AbstractInstruction inst = decode(bytes{151, 128, 129, 63});
    inst.setPC(bytes{0,0,0,0});
    TS_ASSERT(inst.getType() == InstructionType::U);
    inst.execute(&inst, nullptr, 0);
    RegisterFile rf(4, false);
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
    AbstractInstruction inst = decode(bytes{151, 128, 129, 255});
    inst.setPC(bytes{1,12,0,2});
    TS_ASSERT(inst.getType() == InstructionType::U);
    inst.execute(&inst, nullptr, 0);
    RegisterFile rf(4, false);
    inst.registerWriteback(&inst, &rf);
    bytes result{1, 140, 129, 1};
    TS_ASSERT(rf.get(1) == result);
  }

  // void testPosJal(void) {
  //   // J-Type
  //   // imm: 00001000001000000110
  //   // 1111011 10000 00000011 1 0000100000 0
  //   // 1111 0111 0000 0000 0011 1000 0100 0000
  //   DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 111);
  //   AbstractInstruction inst = decode(bytes{239, 0, 28, 2});
  //   inst.setPC(bytes{1,1,1,1});
  //   TS_ASSERT(inst.getType() == InstructionType::J);
  //   inst.execute(&inst, nullptr, 0);
  //   RegisterFile rf(4, false);
  //   inst.registerWriteback(&inst, &rf);
  //   // res: 0000 0000 0000 0001 1000 0001 1111 1100
  //   bytes result{0, 128, 129, 63};
  //   TS_ASSERT(rf.get(1) == result);
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

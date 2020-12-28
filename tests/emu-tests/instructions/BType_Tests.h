#include "../../framework/cxxtest-4.4/cxxtest/TestSuite.h"
#include "../../../src/include/instructions/BType.h"

// TODO: Fix makefile not including the cpp file

class BTypeInstructionTests : public CxxTest::TestSuite
{
  public:
  void testCreateFromNothing(void) {
    TS_ASSERT_THROWS_NOTHING(BTypeInstruction b = BTypeInstruction());
  }

  void testCreateFromArgs(void) {
    BTypeInstruction b = BTypeInstruction(127, 1, 15, 7, 31, 31, 63, 1);
    TS_ASSERT(b.getOpcode() == 127);
    TS_ASSERT(b.getImm(7, 7)[0] == 1);
    TS_ASSERT(b.getImm(8, 11)[0] == 15);
    TS_ASSERT(b.getFunc3() == 7);
    TS_ASSERT(b.getRS1() == 31);
    TS_ASSERT(b.getRS2() == 31);
    TS_ASSERT(b.getImm(25, 30)[0] == 63);
    TS_ASSERT(b.getImm(31, 31)[0] == 1);
  }

  void testDecode(void) {
    // 1101001 0 1001 101 11001 00110 100100 1
    // 1101 0010 1001 1011 1001 0011 0100 1001
    // imm: 0100 1100 1000 1
    BTypeInstruction b = BTypeInstruction();
    b.decode(bytes{75, 217, 201, 146});
    TS_ASSERT(b.getOpcode() == 75);
    TS_ASSERT(b.getImm(7, 7)[0] == 0);
    TS_ASSERT(b.getImm(8, 11)[0] == 9);
    TS_ASSERT(b.getFunc3() == 5);
    TS_ASSERT(b.getRS1() == 19);
    TS_ASSERT(b.getRS2() == 12);
    TS_ASSERT(b.getImm(25, 30)[0] == 9);
    TS_ASSERT(b.getImm(31, 31)[0] == 1);
  }

  void testCreateWithLargeOpcode(void) {
    TS_ASSERT_THROWS(BTypeInstruction b = BTypeInstruction(128, 0, 0, 0, 0, 0, 0, 0), InstructionException*);
  }

  void testCreateWithLargeImm1(void) {
    TS_ASSERT_THROWS(BTypeInstruction b = BTypeInstruction(0, 2, 0, 0, 0, 0, 0, 0), InstructionException*);
  }

  void testCreateWithLargeImm4(void) {
    TS_ASSERT_THROWS(BTypeInstruction b = BTypeInstruction(0, 0, 16, 0, 0, 0, 0, 0), InstructionException*);
  }

  void testCreateWithLargeFunc3(void) {
    TS_ASSERT_THROWS(BTypeInstruction b = BTypeInstruction(0, 0, 0, 8, 0, 0, 0, 0), InstructionException*);
  }

  void testCreateWithLargeRS1(void) {
    TS_ASSERT_THROWS(BTypeInstruction b = BTypeInstruction(0, 0, 0, 0, 32, 0, 0, 0), InstructionException*);
  }

  void testCreateWithLargeRS2(void) {
    TS_ASSERT_THROWS(BTypeInstruction b = BTypeInstruction(0, 0, 0, 0, 0, 32, 0, 0), InstructionException*);
  }

  void testCreateWithLargeImm6(void) {
    TS_ASSERT_THROWS(BTypeInstruction b = BTypeInstruction(0, 0, 0, 0, 0, 0, 64, 0), InstructionException*);
  }

  void testCreateWithLargeImm31(void) {
    TS_ASSERT_THROWS(BTypeInstruction b = BTypeInstruction(0, 0, 0, 0, 0, 0, 0, 2), InstructionException*);
  }

  void testDecodeWithWrongSizeInstruction(void) {
    BTypeInstruction b = BTypeInstruction();
    TS_ASSERT_THROWS(b.decode(bytes{0, 255, 0, 255, 0}), InstructionException*);
  }

  void testGetImmWrongPosition(void) {
    BTypeInstruction b = BTypeInstruction(0, 0, 0, 0, 0, 0, 0, 0);
    TS_ASSERT_THROWS(b.getImm(0, 1), InstructionException*);
  }
};

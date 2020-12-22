#include "../../framework/cxxtest-4.4/cxxtest/TestSuite.h"
#include "../../../src/include/instructions/SType.h"


class STypeInstructionTests : public CxxTest::TestSuite
{
  public:
  void testCreateFromNothing(void) {
    TS_ASSERT_THROWS_NOTHING(STypeInstruction s = STypeInstruction());
  }

  void testCreateFromArgs(void) {
    STypeInstruction s = STypeInstruction(127, 31, 7, 31, 31, 127);
    TS_ASSERT(s.getOpcode() == 127);
    TS_ASSERT(s.getImm(7, 11)[0] == 31);
    TS_ASSERT(s.getFunc3() == 7);
    TS_ASSERT(s.getRS1() == 31);
    TS_ASSERT(s.getRS2() == 31);
    TS_ASSERT(s.getImm(25, 31)[0] == 127);
  }

  void testDecode(void) {
    // 0     6 7   1 2 4 5   9 0   4 5     1
    // 1110111 01100 111 00100 11110 1010111
    // 1110 1110 1100 1110 0100 1111 0101 0111
    STypeInstruction s = STypeInstruction();
    s.decode(bytes{119, 115, 242, 234});
    TS_ASSERT(s.getOpcode() == 119);
    TS_ASSERT(s.getImm(7, 11)[0] == 6);
    TS_ASSERT(s.getFunc3() == 7);
    TS_ASSERT(s.getRS1() == 4);
    TS_ASSERT(s.getRS2() == 15);
    TS_ASSERT(s.getImm(25, 31)[0] == 117);
  }

  void testCreateWithLargeOpcode(void) {
    TS_ASSERT_THROWS(STypeInstruction s = STypeInstruction(128, 0, 0, 0, 0, 0), AbstractInstructionException*);
  }

  void testCreateWithLargeImm5(void) {
    TS_ASSERT_THROWS(STypeInstruction s = STypeInstruction(0, 32, 0, 0, 0, 0), AbstractInstructionException*);
  }

  void testCreateWithLargeFunc3(void) {
    TS_ASSERT_THROWS(STypeInstruction s = STypeInstruction(0, 0, 8, 0, 0, 0), AbstractInstructionException*);
  }

  void testCreateWithLargeRS1(void) {
    TS_ASSERT_THROWS(STypeInstruction s = STypeInstruction(0, 0, 0, 32, 0, 0), AbstractInstructionException*);
  }

  void testCreateWithLargeRS2(void) {
    TS_ASSERT_THROWS(STypeInstruction s = STypeInstruction(0, 0, 0, 0, 32, 0), AbstractInstructionException*);
  }

  void testCreateWithLargeImm7(void) {
    TS_ASSERT_THROWS(STypeInstruction s = STypeInstruction(0, 0, 0, 0, 0, 128), AbstractInstructionException*);
  }

  void testDecodeWithWrongSizeInstruction(void) {
    STypeInstruction s = STypeInstruction();
    TS_ASSERT_THROWS(s.decode(bytes{0, 255}), AbstractInstructionException*);
  }

  void testGetImmAtWrongPosition(void) {
    STypeInstruction s = STypeInstruction();
    TS_ASSERT_THROWS(s.getImm(0, 1), AbstractInstructionException*);
  }
};

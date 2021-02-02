#include "../../framework/cxxtest-4.4/cxxtest/TestSuite.h"
#include "../../../src/include/instructions/RType.h"

// TODO: Fix makefile not including the cpp file

class RTypeInstructionTests : public CxxTest::TestSuite
{
  public:
  void testCreateFromNothing(void) {
    TS_ASSERT_THROWS_NOTHING(RTypeInstruction r = RTypeInstruction(4));
  }

  void testCreateFromArgs(void) {
    RTypeInstruction r = RTypeInstruction(4, 127, 31, 7, 31, 31, 127);
    TS_ASSERT(r.getOpcode() == 127);
    TS_ASSERT(r.getRD() == 31);
    TS_ASSERT(r.getFunc3() == 7);
    TS_ASSERT(r.getRS1() == 31);
    TS_ASSERT(r.getRS2() == 31);
    TS_ASSERT(r.getFunc7() == 127);
  }

  void testDecode(void) {
    // 0     6 7   1 2 4 5   9 0   4 5     1
    // 0110110 00000 010 11110 11111 1000010
    // 0110 1100 0000 0101 1110 1111 1100 0010
    RTypeInstruction r = RTypeInstruction(4);
    r.decode(bytes{54, 160, 247, 67});
    TS_ASSERT(r.getOpcode() == 54);
    TS_ASSERT(r.getRD() == 0);
    TS_ASSERT(r.getFunc3() == 2);
    TS_ASSERT(r.getRS1() == 15);
    TS_ASSERT(r.getRS2() == 31);
    TS_ASSERT(r.getFunc7() == 33);
  }

  void testCreateWithLargeOpcode(void) {
    TS_ASSERT_THROWS_ANYTHING(RTypeInstruction r = RTypeInstruction(4, 128, 31, 7, 31, 31, 127));
  }

  void testCreateWithLargeRD(void) {
    TS_ASSERT_THROWS_ANYTHING(RTypeInstruction r = RTypeInstruction(4, 127, 32, 7, 31, 31, 127));
  }

  void testCreateWithLargeFunc3(void) {
    TS_ASSERT_THROWS_ANYTHING(RTypeInstruction r = RTypeInstruction(4, 127, 31, 8, 31, 31, 127));
  }

  void testCreateWithLargeRS1(void) {
    TS_ASSERT_THROWS_ANYTHING(RTypeInstruction r = RTypeInstruction(4, 127, 31, 7, 32, 31, 127));
  }

  void testCreateWithLargeRS2(void) {
    TS_ASSERT_THROWS_ANYTHING(RTypeInstruction r = RTypeInstruction(4, 127, 31, 7, 31, 32, 127));
  }

  void testCreateWithLargeFunc7(void) {
    TS_ASSERT_THROWS_ANYTHING(RTypeInstruction r = RTypeInstruction(4, 127, 31, 7, 31, 31, 128));
  }

  void testDecodeWithBadInstruction(void) {
    RTypeInstruction r = RTypeInstruction(4);
    TS_ASSERT_THROWS_ANYTHING(r.decode(bytes{255, 255}));
  }

  void testGetImm(void) {
    RTypeInstruction r = RTypeInstruction(4);
    TS_ASSERT_THROWS(r.getImm(0, 1), InstructionException);
  }
};

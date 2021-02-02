#include "../../framework/cxxtest-4.4/cxxtest/TestSuite.h"
#include "../../../src/include/instructions/UType.h"


class UTypeInstructionTests : public CxxTest::TestSuite
{
  public:
  void testCreateFromNothing(void) {
    TS_ASSERT_THROWS_NOTHING(UTypeInstruction u = UTypeInstruction(4));
  }

  void testCreateFromArgs(void) {
    bytes imm = bytes{255, 255, 15};
    UTypeInstruction u = UTypeInstruction(4, 127, 31, imm);
    TS_ASSERT(u.getOpcode() == 127);
    TS_ASSERT(u.getRD() == 31);
    TS_ASSERT(u.getImm(12, 31) == imm);
  }

  void testDecode(void) {
    // 0     6 7   1 2                  1
    // 0010001 11110 11010110101100010101
    // 0010 0011 1110 1101 0110 1011 0001 0101
    UTypeInstruction u = UTypeInstruction(4);
    u.decode(bytes{196, 183, 214, 168});
    TS_ASSERT(u.getOpcode() == 68);
    TS_ASSERT(u.getRD() == 15);
    bytes imm = bytes{107, 141, 10};
    TS_ASSERT(u.getImm(12, 31) == imm);
  }

  void testCreateWithLargeOpcode(void) {
    TS_ASSERT_THROWS(UTypeInstruction u = UTypeInstruction(4, 128, 0, bytes{0, 0, 0}), InstructionException*);
  }

  void testCreateWithLargeRD(void) {
    TS_ASSERT_THROWS(UTypeInstruction u = UTypeInstruction(4, 0, 32, bytes{0, 0, 0}), InstructionException*);
  }

  void testCreateWithLargeImm(void) {
    TS_ASSERT_THROWS(UTypeInstruction u = UTypeInstruction(4, 0, 0, bytes{255, 255, 16}), InstructionException*);
  }

  void testDecodeWithWrongSizeInstruction(void) {
    UTypeInstruction u = UTypeInstruction(4);
    TS_ASSERT_THROWS(u.decode(bytes{255, 255, 255, 255, 255}), InstructionException*);
  }

  void testGetImmAtWrongPosition(void) {
    UTypeInstruction u = UTypeInstruction(4, 127, 31, bytes{255, 255, 15});
    TS_ASSERT_THROWS(u.getImm(0, 1), InstructionException*);
  }
};

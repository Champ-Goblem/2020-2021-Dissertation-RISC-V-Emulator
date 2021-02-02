#include "../../framework/cxxtest-4.4/cxxtest/TestSuite.h"
#include "../../../src/include/instructions/IType.h"

// TODO: Fix makefile not including the cpp file

class ITypeInstructionTests : public CxxTest::TestSuite
{
  public:
  void testCreateFromNothing(void) {
    TS_ASSERT_THROWS_NOTHING(ITypeInstruction i = ITypeInstruction(4));
  }

  void testCreateFromArgs(void) {
    ITypeInstruction i = ITypeInstruction(4, 127, 31, 7, 31, bytes{255, 15});
    TS_ASSERT(i.getOpcode() == 127);
    TS_ASSERT(i.getRD() == 31);
    TS_ASSERT(i.getFunc3() == 7);
    TS_ASSERT(i.getRS1() == 31);
    bytes imm = bytes{255, 255};
    TS_ASSERT(i.AbstractInstruction::getImm() == imm);
  }

  void testDecode(void) {
    // 0     6 7   1 2 4 5   9 0          1
    // 1010111 10111 000 00011 101100001111
    // 1010 1111 0111 0000 0011 1011 0000 1111
    ITypeInstruction i = ITypeInstruction(4);
    i.decode(bytes{245, 14, 220, 240});
    TS_ASSERT(i.getOpcode() == 117);
    TS_ASSERT(i.getRD() == 29);
    TS_ASSERT(i.getFunc3() == 0);
    TS_ASSERT(i.getRS1() == 24);
    bytes imm = bytes{13, 255};
    TS_ASSERT(i.AbstractInstruction::getImm() == imm);
  }

  void testCreateWithLargeOpcode(void) {
    TS_ASSERT_THROWS(ITypeInstruction i = ITypeInstruction(4, 128, 0, 0, 0, bytes{0, 0}), InstructionException*);
  }

  void testCreateWithLargeRD(void) {
    TS_ASSERT_THROWS(ITypeInstruction i = ITypeInstruction(4, 0, 32, 0, 0, bytes{0, 0}), InstructionException*);
  }

  void testCreateWithLargeFunc3(void) {
    TS_ASSERT_THROWS(ITypeInstruction i = ITypeInstruction(4, 0, 0, 10, 0, bytes{0, 0}), InstructionException*);
  }

  void testCreateWithLargeRS1(void) {
    TS_ASSERT_THROWS(ITypeInstruction i = ITypeInstruction(4, 0, 0, 0, 32, bytes{0, 0}), InstructionException*);
  }

  void testCreateWithLargeImm(void) {
    TS_ASSERT_THROWS(ITypeInstruction i = ITypeInstruction(4, 0, 0, 0, 0, bytes{255, 255}), InstructionException*);
  }

  void testCreateWithWrongSizeImm(void) {
    TS_ASSERT_THROWS(ITypeInstruction i = ITypeInstruction(4, 0, 0, 0, 0, bytes{0, 0, 0}), InstructionException*);
  }

  void testDecodeWithWrongSizeInstruction(void) {
    ITypeInstruction i = ITypeInstruction(4);
    TS_ASSERT_THROWS(i.decode(bytes{0, 255, 0, 255, 0}), InstructionException*);
  }

  void testGetImm(void) {
    bytes imm = bytes{255, 0};
    ITypeInstruction i = ITypeInstruction(4, 0, 0, 0, 0, imm);
    TS_ASSERT(i.getImm(20, 31) == imm);
  }

  void testGetImmWrongPosition(void) {
    ITypeInstruction i = ITypeInstruction(4, 0, 0, 0, 0, bytes{255, 0});
    TS_ASSERT_THROWS(i.getImm(0, 1), InstructionException*);
  }
};

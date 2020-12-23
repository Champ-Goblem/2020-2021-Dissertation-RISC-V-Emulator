#include "../../framework/cxxtest-4.4/cxxtest/TestSuite.h"
#include "../../../src/include/instructions/JType.h"


class JTypeInstructionTests : public CxxTest::TestSuite
{
  public:
  void testCreateFromNothing(void) {
    TS_ASSERT_THROWS_NOTHING(JTypeInstruction j = JTypeInstruction());
  }

  void testCreateFromArgs(void) {
    bytes imm10 = bytes{255, 3};
    JTypeInstruction j = JTypeInstruction(127, 31, 255, 1, imm10, 1);
    TS_ASSERT(j.getOpcode() == 127);
    TS_ASSERT(j.getRD() == 31);
    TS_ASSERT(j.getImm(12, 19)[0] == 255);
    TS_ASSERT(j.getImm(20, 20)[0] == 1);
    TS_ASSERT(j.getImm(21, 30) == imm10);
    TS_ASSERT(j.getImm(31, 31)[0] == 1);
  }

  void testDecode(void) {
    // 1011101 00111 11001100 0 0101011100 1
    // 1011 1010 0111 1100 1100 0010 1011 1001
    JTypeInstruction j = JTypeInstruction();
    j.decode(bytes{93, 62, 67, 157});
    TS_ASSERT(j.getOpcode() == 93);
    TS_ASSERT(j.getRD() == 28);
    TS_ASSERT(j.getImm(12, 19)[0] == 51);
    TS_ASSERT(j.getImm(20, 20)[0] == 0);
    bytes imm10 = bytes{234, 0};
    TS_ASSERT(j.getImm(21, 30) == imm10);
    TS_ASSERT(j.getImm(31, 31)[0] == 1); 
  }
};

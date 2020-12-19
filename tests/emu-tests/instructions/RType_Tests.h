#include "../../framework/cxxtest-4.4/cxxtest/TestSuite.h"
#include "../../../src/include/instructions/RType.h"

// TODO: Fix makefile not including the cpp file

class RTypeInstructionTests : public CxxTest::TestSuite
{
  public:
  void testCreateFromNothing(void) {
    TS_ASSERT_THROWS_NOTHING(RTypeInstruction r = RTypeInstruction());
  }

  void testCreateFromArgs(void) {
    RTypeInstruction r = RTypeInstruction(127, 31, 7, 31, 31, 127);
    TS_ASSERT(r.getOpcode() == 127);
    TS_ASSERT(r.getRD() == 31);
    TS_ASSERT(r.getFunc3() == 7);
    TS_ASSERT(r.getRS1() == 31);
    TS_ASSERT(r.getRS2() == 31);
    TS_ASSERT(r.getFunc7() == 127);
  }
};

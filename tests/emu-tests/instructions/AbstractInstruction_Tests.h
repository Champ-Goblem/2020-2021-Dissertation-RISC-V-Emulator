#include "../../framework/cxxtest-4.4/cxxtest/TestSuite.h"
#include "../../../src/include/instructions/AbstractInstruction.h"

class AbstractInstructionTester: public AbstractInstruction {
  public:
  AbstractInstructionTester(bytes imm) {
    this->imm = imm;
  }
};

class RegisterFileTests : public CxxTest::TestSuite
{
  public:
  void testGetConstrainedImm(void) {
    // 0000 0000   0000 1000   1110 1000   0010 0000
    AbstractInstructionTester i = AbstractInstructionTester(bytes {0, 16, 23, 4});
    bytes ans = i.getImm(12, 26);
    TS_ASSERT(ans == (bytes{113, 65}));
  }

  void testGetConstrainedImmHighLtnOffset(void) {
    // 1000 1110
    AbstractInstructionTester i = AbstractInstructionTester(bytes {113, 0, 0, 0});
    bytes ans = i.getImm(0, 6);
    TS_ASSERT(ans == (bytes{49}));
  }
};

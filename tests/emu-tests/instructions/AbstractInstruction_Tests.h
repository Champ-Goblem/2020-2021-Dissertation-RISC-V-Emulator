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
    AbstractInstructionTester* i = new AbstractInstructionTester(bytes {0, 16, 23, 4});
    bytes ans = i->getImm(12, 26);
    // printf(getBytesForPrint(ans).c_str());
    TS_ASSERT(ans == (bytes{23, 65}));
  }
};

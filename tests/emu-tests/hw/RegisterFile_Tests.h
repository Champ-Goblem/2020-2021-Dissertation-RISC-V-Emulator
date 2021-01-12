#include "../../framework/cxxtest-4.4/cxxtest/TestSuite.h"
#include "../../../src/include/hw/RegisterFile.h"

class RegisterFileTests : public CxxTest::TestSuite
{
  public:
    void testRV32ERegisterCount(void) {
      RegisterFile* rf = new RegisterFile((ushort)4, true);
      for (int i=0; i<REG16; i++) {
        try {
          rf->get(i);
        } catch (RegisterFileException e) {
          TS_FAIL("Not all 15 registers present");
        }
      }
      delete rf;
    }

    void testNonRV32ERegisterCount(void) {
      RegisterFile* rf = new RegisterFile((ushort)4, false);
      for (int i=0; i<REG32; i++) {
        try {
          rf->get(i);
        } catch (RegisterFileException e) {
          TS_FAIL("Not all 32 registers present");
        }
      }
      delete rf;

    }

    void testSize(void) {
      RegisterFile* rf = new RegisterFile((ushort)4, true);
      bytes b = rf->get(1);
      TS_ASSERT(b.size() == 4);
      delete rf;
    }

    void testRegisterFunction(void) {
      RegisterFile* rf = new RegisterFile((ushort)4, true);
      bytes b = bytes {1,2,3,3};
      rf->write(1, b);
      TS_ASSERT(rf->get(1) == b);
      delete rf;
    }

    void testRegisterZero(void) {
      RegisterFile* rf = new RegisterFile((ushort)4, true);
      bytes b = bytes {1,2,3,4};
      rf->write(0, b);
      TS_ASSERT(rf->get(0) != b);
      delete rf;
    }

    void testWriteWrongSize(void) {
      RegisterFile* rf = new RegisterFile((ushort)2, true);
      bytes b = bytes {1,2,3,4};
      TS_ASSERT_THROWS_ANYTHING(rf->write(1, b));
      delete rf;
    }

    void testWriteToHighReg(void) {
      RegisterFile* rf = new RegisterFile((ushort)4, true);
      bytes b = bytes {1,2,3,4};
      TS_ASSERT_THROWS_ANYTHING(rf->write(50, b));
      delete rf;
    }

    // void testPC(void) {
    //   RegisterFile* rf = new RegisterFile((ushort)4, true);
    //   bytes b = bytes {1,2,3,4};
    //   rf->writePC(b);
    //   TS_ASSERT(rf->getPC() == b);
    //   delete rf;
    // }

    // void testWrongSizePCWrite(void) {
    //   RegisterFile* rf = new RegisterFile((ushort)2, true);
    //   bytes b = bytes {1,2,3,4};
    //   TS_ASSERT_THROWS_ANYTHING(rf->writePC(b));
    //   delete rf;
    // }
};
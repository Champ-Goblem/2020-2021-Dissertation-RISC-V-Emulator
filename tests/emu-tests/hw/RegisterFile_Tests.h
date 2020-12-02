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
    }

    void testNonRV32ERegisterCount(void) {
      RegisterFile* rf = new RegisterFile((ushort)4, true);
      for (int i=0; i<REG32; i++) {
        try {
          rf->get(i);
        } catch (RegisterFileException e) {
          TS_FAIL("Not all 32 registers present");
        }
      }
    }
};
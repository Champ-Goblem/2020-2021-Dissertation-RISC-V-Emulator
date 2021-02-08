#include "../../framework/cxxtest-4.4/cxxtest/TestSuite.h"
#include "../../../src/include/units/PipelineHazardController.h"
#include "../../../src/include/instructions/RType.h"
#include "../../../src/include/hw/RegisterFile.h"

class PipelineHazardControllerTests : public CxxTest::TestSuite {
  public:

  void testCreate(void) {
    RegisterFile rf(4, false);
    TS_ASSERT_THROWS_NOTHING(PipelineHazardController(4, &rf, false));
  }

  void testEnqueue(void) {
    RegisterFile rf(4, false);
    PipelineHazardController phc(4, &rf, false);
    RTypeInstruction r = RTypeInstruction(4);
    TS_ASSERT_THROWS_NOTHING(phc.enqueue((AbstractInstruction*)&r));
  }

  void testCheckStaleWithSize1(void) {
    RegisterFile rf(4, false);
    PipelineHazardController phc(4, &rf, false);
    RTypeInstruction r = RTypeInstruction(4, 0, 1, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r);
    TS_ASSERT(phc.checkForStaleRegister(1) == false);
  }

  void testCheckStaleWithDep(void) {
    RegisterFile rf(4, false);
    PipelineHazardController phc(4, &rf, false);
    RTypeInstruction r = RTypeInstruction(4, 0, 1, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r);
    RTypeInstruction r1 = RTypeInstruction(4, 0, 0, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r1);
    TS_ASSERT(phc.checkForStaleRegister(1) == true);
  }

  void testCheckStaleWithNoDep(void) {
    RegisterFile rf(4, false);
    PipelineHazardController phc(4, &rf, false);
    RTypeInstruction r = RTypeInstruction(4, 0, 1, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r);
    RTypeInstruction r1 = RTypeInstruction(4, 0, 0, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r1);
    TS_ASSERT(phc.checkForStaleRegister(2) == false);
  }
  
  void testCheckStaleWithDepOnX0(void) {
    RegisterFile rf(4, false);
    PipelineHazardController phc(4, &rf, false);
    RTypeInstruction r = RTypeInstruction(4, 0, 1, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r);
    RTypeInstruction r1 = RTypeInstruction(4, 0, 0, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r1);
    TS_ASSERT(phc.checkForStaleRegister(0) == false);
  }

  void testFetchRegisterValueWithSize1(void) {
    RegisterFile rf(4, false);
    bytes val{255, 0, 0, 0};
    rf.write(4, val);
    PipelineHazardController phc(4, &rf, false);
    RTypeInstruction r = RTypeInstruction(4, 0, 1, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r);
    TS_ASSERT(phc.fetchRegisterValue(4) == val);
  }

  void testFetchRegisterValueWithSize2Dep(void) {
    RegisterFile rf(4, false);
    bytes val{255, 0, 0, 0};
    rf.write(4, val);
    PipelineHazardController phc(4, &rf, false);
    RTypeInstruction r = RTypeInstruction(4, 0, 1, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r);
    RTypeInstruction r1 = RTypeInstruction(4, 0, 2, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r1);
    TS_ASSERT_THROWS(phc.fetchRegisterValue(1), PipelineHazardException);
  }

  void testFetchRegisterValueWithSize2NoDep(void) {
    RegisterFile rf(4, false);
    bytes val{255, 0, 0, 0};
    rf.write(4, val);
    PipelineHazardController phc(4, &rf, false);
    RTypeInstruction r = RTypeInstruction(4, 0, 1, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r);
    RTypeInstruction r1 = RTypeInstruction(4, 0, 2, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r1);
    TS_ASSERT(phc.fetchRegisterValue(4) == val);
  }

  void testFetchRegisterValueWithSize3Dep(void) {
    RegisterFile rf(4, false);
    bytes val{255, 255, 0, 0};
    PipelineHazardController phc(4, &rf, false);
    RTypeInstruction r = RTypeInstruction(4, 0, 1, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r);
    RTypeInstruction r1 = RTypeInstruction(4, 0, 2, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r1);
    phc.storeResultAfterExecution(val);
    RTypeInstruction r2 = RTypeInstruction(4, 0, 3, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r2);
    TS_ASSERT(phc.fetchRegisterValue(1) == val);
  }

  void testFetchRegisterValueWithSize3NoDep(void) {
    RegisterFile rf(4, false);
    bytes val{255, 0, 0, 0};
    rf.write(4, val);
    PipelineHazardController phc(4, &rf, false);
    RTypeInstruction r = RTypeInstruction(4, 0, 1, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r);
    RTypeInstruction r1 = RTypeInstruction(4, 0, 2, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r1);
    RTypeInstruction r2 = RTypeInstruction(4, 0, 3, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r2);
    phc.storeResultAfterExecution(bytes{255, 255, 0, 0});
    TS_ASSERT(phc.fetchRegisterValue(4) == val);
  }

  void testFetchRegisterValueWithSize4Dep(void) {
    RegisterFile rf(4, false);
    bytes val{255, 255, 0, 0};
    PipelineHazardController phc(4, &rf, false);
    RTypeInstruction r = RTypeInstruction(4, 0, 1, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r);
    RTypeInstruction r1 = RTypeInstruction(4, 0, 2, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r1);
    phc.storeResultAfterExecution(val);
    RTypeInstruction r2 = RTypeInstruction(4, 0, 3, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r2);
    RTypeInstruction r3 = RTypeInstruction(4, 0, 4, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r3);
    TS_ASSERT(phc.fetchRegisterValue(1) == val);
  }

  void testFetchRegisterValueWithSize4NoDep(void) {
    RegisterFile rf(4, false);
    bytes val{255, 0, 0, 0};
    rf.write(5, val);
    PipelineHazardController phc(4, &rf, false);
    RTypeInstruction r = RTypeInstruction(4, 0, 1, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r);
    RTypeInstruction r1 = RTypeInstruction(4, 0, 2, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r1);
    RTypeInstruction r2 = RTypeInstruction(4, 0, 3, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r2);
    RTypeInstruction r3 = RTypeInstruction(4, 0, 4, 0, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r3);
    phc.storeResultAfterExecution(bytes{255, 255, 0, 0});
    TS_ASSERT(phc.fetchRegisterValue(5) == val);
  }

  void testStoreResultAfterExecute(void) {
    RegisterFile rf(4, false);
    bytes val{255, 0, 0, 0};
    PipelineHazardController phc(4, &rf, false);
    RTypeInstruction r = RTypeInstruction(4, 0, 1, 2, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r);
    RTypeInstruction r1 = RTypeInstruction(4, 0, 2, 1, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r1);
    phc.storeResultAfterExecution(val);
    phc.enqueue((AbstractInstruction*)&r1);
    TS_ASSERT(phc.fetchRegisterValue(1) == val);
  }

  void testStoreResultAfterMemoryAccess(void) {
    RegisterFile rf(4, false);
    bytes val{255, 0, 0, 0};
    PipelineHazardController phc(4, &rf, false);
    RTypeInstruction r = RTypeInstruction(4, 0, 1, 2, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r);
    RTypeInstruction r1 = RTypeInstruction(4, 0, 2, 1, 0, 0, 0);
    phc.enqueue((AbstractInstruction*)&r1);
    phc.enqueue((AbstractInstruction*)&r);
    phc.storeResultAfterMemoryAccess(val);
    TS_ASSERT(phc.fetchRegisterValue(1) == val);
  }
};
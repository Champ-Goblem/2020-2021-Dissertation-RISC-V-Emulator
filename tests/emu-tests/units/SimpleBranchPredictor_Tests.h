#include "../../framework/cxxtest-4.4/cxxtest/TestSuite.h"
#include "../../../src/include/units/SimpleBranchPredictor.h"

class SimpleBranchPredictorTests : public CxxTest::TestSuite {
  // Test with initial PC point to out of mem
  // Test with jumps to out of mem
  // Test with misaligned
  // Test with initial pc>0
  public:
  void testCreate(void) {
    Memory m(100);
    RegisterFile rf(4, false);
    TS_ASSERT_THROWS_NOTHING(SimpleBranchPredictor s(&m, 4, &rf, bytes{0,0,0,0}));
  }

  void testGetPC(void) {
    Memory m(100);
    RegisterFile rf(4, false);
    SimpleBranchPredictor s(&m, 4, &rf, bytes{0,0,0,0});
    bytes nextPC = bytes{4,0,0,0};
    s.getNextPC();
    TS_ASSERT(s.getNextPC() == nextPC);
  }

  void testGetPCWithPosBranch(void) {
    // Uses B-Type encoding
    // imm: 000100000000
    // 1100011 0 0010 000 00000 00000 000000 0
    // 1100 0110 0010 0000 0000 0000 0000 0000
    // Should only do PC+4
    Memory m(100);
    m.writeDWord(4, bytes{99, 4, 0, 0});
    RegisterFile rf(4, false);
    SimpleBranchPredictor s(&m, 4, &rf, bytes{0,0,0,0});
    s.getNextPC();
    s.getNextPC();
    bytes nextPC = bytes{8,0,0,0};
    TS_ASSERT(s.getNextPC() == nextPC);
  }

  void testGetPCWithNegBranch(void) {
    // Uses B-Type encoding
    // imm: 0010111111111
    // 1100011 1 0101 000 00000 00000 111111 1
    // 1100 0111 0101 0000 0000 0000 0111 1111
    Memory m(100);
    m.writeDWord(16, bytes{227, 10, 0, 254});
    RegisterFile rf(4, false);
    SimpleBranchPredictor s(&m, 4, &rf, bytes{16,0,0,0});
    bytes nextPC = bytes{4,0,0,0};
    s.getNextPC();
    TS_ASSERT(s.getNextPC() == nextPC);
  }

  void testGetPCWithPosJALR(void) {
    // Uses I-Type
    // imm: 000100000000
    // 1100111 00000 000 00000 000100000000
    // 1100 1110 0000 0000 0000 0001 0000 0000
    // RS1: 0 -> 0
    Memory m(100);
    m.writeDWord(0, bytes{103, 0, 128, 0});
    RegisterFile rf(4, false);
    SimpleBranchPredictor s(&m, 4, &rf, bytes{0,0,0,0});
    s.getNextPC();
    bytes nextPC = bytes{8,0,0,0};
    TS_ASSERT(s.getNextPC() == nextPC);
  }

  void testGetPCWithNegJALR(void) {
    // Uses I-Type
    // imm: 000111111111
    // 1100111 00000 000 10000 000111111111
    // 1100 1110 0000 0001 0000 0001 1111 1111
    // RS1 1 -> 8
    Memory m(100);
    m.writeDWord(0, bytes{103, 128, 128, 255});
    RegisterFile rf(4, false);
    rf.write(1, bytes{8,0,0,0});
    SimpleBranchPredictor s(&m, 4, &rf, bytes{0,0,0,0});
    s.getNextPC();
    bytes nextPC = bytes{0,0,0,0};
    TS_ASSERT(s.getNextPC() == nextPC);
  }

  void testGetPCWithPosJAL(void) {
    // Uses J-Type
    // imm: 000100000000000000000
    // 1101111 00000 00000000 0 0010000000 0
    // 1101 1110 0000 0000 0000 0001 0000 0000
    Memory m(100);
    m.writeDWord(0, bytes{111, 0, 128, 0});
    RegisterFile rf(4, false);
    SimpleBranchPredictor s(&m, 4, &rf, bytes{0,0,0,0});
    s.getNextPC();
    bytes nextPC = bytes{8,0,0,0};
    TS_ASSERT(s.getNextPC() == nextPC);
  }

  void testGetPCWithNegJAL(void) {
    // Uses J-Type
    // imm: 000111111111111111111
    // 1111011 00000 11111111 1 0011111111 1
    // 1111 0110 0000 1111 1111 1001 1111 1111
    Memory m(100);
    m.writeDWord(16, bytes{111, 240, 159, 255});
    RegisterFile rf(4, false);
    SimpleBranchPredictor s(&m, 4, &rf, bytes{16,0,0,0});
    s.getNextPC();
    bytes nextPC = bytes{8,0,0,0};
    TS_ASSERT(s.getNextPC() == nextPC);
  }

  void testCreateWithOutOfMemoryInstruction(void) {
    Memory m(16);
    RegisterFile rf(4, false);
    TS_ASSERT_THROWS(SimpleBranchPredictor s(&m, 4, &rf, bytes{16,0,0,0}), AddressOutOfMemoryException);
  }

  void testCreateWithWrongSizeInstruction(void) {
    Memory m(16);
    RegisterFile rf(4, false);
    TS_ASSERT_THROWS(SimpleBranchPredictor s(&m, 4, &rf, bytes{0,0,0}), BranchPredictorException);
  }

  void testCreateWithMisalignedInstruction(void) {
    Memory m(16);
    RegisterFile rf(4, false);
    TS_ASSERT_THROWS(SimpleBranchPredictor s(&m, 4, &rf, bytes{1,0,0,0}), AddressMisalignedException);
  }

  // Doesnt seem to be catching the errors below properly
  // void testGetWithJALRToOutOfMemory(void) {
  //   // Uses I-Type
  //   // imm: 001010000000
  //   // 1100111 00000 000 00000 001010000000
  //   // 1100 1110 0000 0000 0000 0010 1000 0000
  //   Memory m(20);
  //   m.writeDWord(0, bytes{115, 0, 64, 1});
  //   RegisterFile rf(4, false);
  //   SimpleBranchPredictor s(&m, 4, &rf, bytes{0,0,0,0});
  //   TS_ASSERT_THROWS(s.getNextPC(), AddressOutOfMemoryException*);
  // }

  // void testGetWithJALToOutOfMemory(void) {
  //   // Uses J-Type
  //   // imm: 00101000000000000000
  //   // 1101111 00000 00000000 0 0101000000 0
  //   // 1101 1110 0000 0000 0000 0010 1000 0000
  //   Memory m(20);
  //   m.writeDWord(0, bytes{123, 0, 64, 1});
  //   RegisterFile rf(4, false);
  //   SimpleBranchPredictor s(&m, 4, &rf, bytes{0,0,0,0});
  //   TS_ASSERT_THROWS(s.getNextPC(), AddressOutOfMemoryException*);
  // }

};
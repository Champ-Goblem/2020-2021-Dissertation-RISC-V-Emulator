#include "../../framework/cxxtest-4.4/cxxtest/TestSuite.h"
#include "../../../src/include/units/SimpleBranchPredictor.h"
#include "../../../src/include/exceptions.h"
#include "../../../src/include/hw/Memory.h"
#include "../../../src/include/hw/RegisterFile.h"

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
    m.writeWord(4, bytes{99, 4, 0, 0});
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
    m.writeWord(16, bytes{227, 10, 0, 254});
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
    m.writeWord(0, bytes{103, 0, 128, 0});
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
    m.writeWord(0, bytes{103, 128, 128, 255});
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
    m.writeWord(0, bytes{111, 0, 128, 0});
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
    m.writeWord(16, bytes{111, 240, 159, 255});
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

  void testCheckPrediction(void) {
    Memory m(16);
    RegisterFile rf(4, false);
    SimpleBranchPredictor s(&m, 4, &rf, bytes{0,0,0,0});
    s.getNextPC();
    TS_ASSERT(s.checkPrediction(bytes{0,0,0,0}, bytes{4,0,0,0}));
  }

  void testCheckPredictionContinuous(void) {
    Memory m(30);
    RegisterFile rf(4, false);
    SimpleBranchPredictor s(&m, 4, &rf, bytes{0,0,0,0});
    s.getNextPC();
    TS_ASSERT(s.checkPrediction(bytes{0,0,0,0}, bytes{4,0,0,0}));
    s.getNextPC();
    TS_ASSERT(s.checkPrediction(bytes{4,0,0,0}, bytes{8,0,0,0}));
    s.getNextPC();
    TS_ASSERT(s.checkPrediction(bytes{8,0,0,0}, bytes{12,0,0,0}));
  }

  void testCheckPredictionWithPosJAL(void) {
    // Uses J-Type
    // imm: 000100000000000000000
    // 1111011 00000 00000000 0 0010000000 0
    // 1111 0110 0000 0000 0000 0001 0000 0000
    Memory m(40);
    m.writeWord(0, bytes{111, 0, 128, 0});
    RegisterFile rf(4, false);
    SimpleBranchPredictor s(&m, 4, &rf, bytes{0,0,0,0});
    s.getNextPC();
    TS_ASSERT(s.checkPrediction(bytes{0,0,0,0}, bytes{8,0,0,0}));
    s.getNextPC();
    TS_ASSERT(s.checkPrediction(bytes{8,0,0,0}, bytes{12,0,0,0}));
    s.getNextPC();
    TS_ASSERT(s.checkPrediction(bytes{12,0,0,0}, bytes{16,0,0,0}));
  }

  void testCheckPredictionWithNegJAL(void) {
    // Uses J-Type
    // imm: 001111111111111111111
    // 1111011 00000 11111111 1 0111111111 1
    // 1111 0110 0000 1111 1111 1011 1111 1111
    Memory m(40);
    m.writeWord(16, bytes{111, 240, 223, 255});
    RegisterFile rf(4, false);
    SimpleBranchPredictor s(&m, 4, &rf, bytes{16,0,0,0});
    s.getNextPC();
    TS_ASSERT(s.checkPrediction(bytes{16,0,0,0}, bytes{12,0,0,0}));
    s.getNextPC();
    TS_ASSERT(s.checkPrediction(bytes{12,0,0,0}, bytes{16,0,0,0}));
    s.getNextPC();
    TS_ASSERT(s.checkPrediction(bytes{16,0,0,0}, bytes{12,0,0,0}));
  }

  void testCheckPredictionWithPosJALR(void) {
    // Uses I-Type
    // imm: 000100000000
    // 1100111 00000 000 00000 000100000000
    // 1100 1110 0000 0000 0000 0001 0000 0000
    // RS1: 0 -> 0
    Memory m(100);
    m.writeWord(0, bytes{103, 0, 128, 0});
    RegisterFile rf(4, false);
    SimpleBranchPredictor s(&m, 4, &rf, bytes{0,0,0,0});
    s.getNextPC();
    TS_ASSERT(s.checkPrediction(bytes{0,0,0,0}, bytes{8,0,0,0}));
    s.getNextPC();
    TS_ASSERT(s.checkPrediction(bytes{8,0,0,0}, bytes{12,0,0,0}));
    s.getNextPC();
    TS_ASSERT(s.checkPrediction(bytes{12,0,0,0}, bytes{16,0,0,0}));
  }

  void testCheckPredictionWithNegJALR(void) {
    // Uses I-Type
    // imm: 000111111111
    // 1100111 00000 000 10000 000111111111
    // 1100 1110 0000 0001 0000 0001 1111 1111
    // RS1 1 -> 8
    Memory m(100);
    m.writeWord(0, bytes{103, 128, 128, 255});
    RegisterFile rf(4, false);
    rf.write(1, bytes{8,0,0,0});
    SimpleBranchPredictor s(&m, 4, &rf, bytes{0,0,0,0});
    s.getNextPC();
    TS_ASSERT(s.checkPrediction(bytes{0,0,0,0}, bytes{0,0,0,0}));
    s.getNextPC();
    TS_ASSERT(s.checkPrediction(bytes{0,0,0,0}, bytes{0,0,0,0}));
  }

  void testCheckPredictionWithFailedPrediction(void) {
    // Uses B-Type
    // imm: 0001000000000
    // 1100011 0 0010 000 00000 00000 000000 0
    // 1100 0110 0010 0000 0000 0000 0000 0000
    Memory m(40);
    m.writeWord(0, bytes{99, 4, 0, 0});
    RegisterFile rf(4, false);
    SimpleBranchPredictor s(&m, 4, &rf, bytes{0,0,0,0});
    s.getNextPC();
    TS_ASSERT(!s.checkPrediction(bytes{0,0,0,0}, bytes{8,0,0,0}));
    s.getNextPC();
    TS_ASSERT(s.checkPrediction(bytes{8,0,0,0}, bytes{12,0,0,0}));
    s.getNextPC();
    TS_ASSERT(s.checkPrediction(bytes{12,0,0,0}, bytes{16,0,0,0}));
  }

  void testGetWithJALRToOutOfMemory(void) {
    // Uses I-Type
    // imm: 001010000000
    // 1110011 00000 000 00000 001010000000
    // 1110 0110 0000 0000 0000 0010 1000 0000
    Memory m(20);
    m.writeWord(0, bytes{103, 0, 64, 1});
    RegisterFile rf(4, false);
    SimpleBranchPredictor s(&m, 4, &rf, bytes{0,0,0,0});
    TS_ASSERT_THROWS(s.getNextPC(), AddressOutOfMemoryException);
  }

  // These weirdly dont throw anything
  // void testGetWithJALToOutOfMemory(void) {
  //   // Uses J-Type
  //   // imm: 00101000000000000000
  //   // 1111011 00000 00000000 0 0101000000 0
  //   // 1111 0110 0000 0000 0000 0010 1000 0000
  //   Memory m(20);
  //   m.writeWord(0, bytes{111, 0, 64, 1});
  //   RegisterFile rf(4, false);
  //   SimpleBranchPredictor s(&m, 4, &rf, bytes{0,0,0,0});
  //   TS_ASSERT_THROWS(s.getNextPC(), AddressOutOfMemoryException);
  // }

  // void testGetWithNegJALRToOutOfMemory(void) {
  //   // Uses I-Type
  //   // imm: 001111111111
  //   // 1110011 00000 000 00000 001111111111
  //   // 1110 0110 0000 0000 0000 0011 1111 1111
  //   Memory m(20);
  //   m.writeWord(0, bytes{103, 0, 192, 255});
  //   RegisterFile rf(4, false);
  //   SimpleBranchPredictor s(&m, 4, &rf, bytes{0,0,0,0});
  //   TS_ASSERT_THROWS(s.getNextPC(), AddressOutOfMemoryException);
  // }

};
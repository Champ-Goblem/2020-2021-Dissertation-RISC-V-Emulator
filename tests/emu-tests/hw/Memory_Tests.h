#include "../../framework/cxxtest-4.4/cxxtest/TestSuite.h"
#include "../../../src/include/hw/Memory.h"
#include "../../../src/include/exceptions.h"

class MemoryTests : public CxxTest::TestSuite {
  public:
  void testMemorySize(void) {
    ulong size = 128;
    Memory* m = new Memory(size);
    for (int i=0; i < size; i++) {
      TS_ASSERT_THROWS_NOTHING(m->readByte(i));
    }
    delete m;
  }

  void testMax(void) {
    ulong size = 128;
    Memory* m = new Memory(size);
    TS_ASSERT_THROWS(m->readByte(size), AddressOutOfMemoryException);
    delete m;
  }

  void testWriteReadByte(void) {
    ulong size = 128;
    Memory* m = new Memory(size);
    m->writeByte(2, 255);
    TS_ASSERT(m->readByte(2) == 255);
    delete m;
  }

  void testWriteReadHWord(void) {
    ulong size = 128;
    Memory* m = new Memory(size);
    bytes b = bytes {255, 0};
    m->writeHWord(2, b);
    TS_ASSERT(m->readHWord(2) == b);
    delete m;
  }

    void testWriteReadWord(void) {
     ulong size = 128;
    Memory* m = new Memory(size);
    bytes b = bytes {255, 0, 255, 0};
    m->writeWord(2, b);
    TS_ASSERT(m->readWord(2) == b);
    delete m;
  }

  void testWriteReadDWord(void) {
    ulong size = 128;
    Memory* m = new Memory(size);
    bytes b = bytes {255, 0, 255, 0, 255, 0, 255, 0};
    m->writeDWord(2, b);
    TS_ASSERT(m->readDWord(2) == b);
    delete m;
  }

  void testWriteReadQWord(void) {
    ulong size = 128;
    Memory* m = new Memory(size);
    bytes b = bytes {255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0};
    m->writeQWord(2, b);
    TS_ASSERT(m->readQWord(2) == b);
    delete m;
  }

  void testWrongSizeWriteHWord(void) {
    ulong size = 128;
    Memory* m = new Memory(size);
    bytes b = bytes {255, 0, 255};
    TS_ASSERT_THROWS(m->writeHWord(2, b), WrongSizeInstructionException);
    delete m;
  }


  void testWrongSizeWriteWord(void) {
    ulong size = 128;
    Memory* m = new Memory(size);
    bytes b = bytes {255, 0, 255};
    TS_ASSERT_THROWS(m->writeWord(2, b), WrongSizeInstructionException);
    delete m;
  }

  void testWrongSizeWriteDWord(void) {
    ulong size = 128;
    Memory* m = new Memory(size);
    bytes b = bytes {255, 0, 255, 0, 255};
    TS_ASSERT_THROWS(m->writeDWord(2, b), WrongSizeInstructionException);
    delete m;
  }

  void testWrongSizeWriteQWord(void) {
    ulong size = 128;
    Memory* m = new Memory(size);
    bytes b = bytes {255, 0, 255, 0, 255, 0, 255, 0, 255};
    TS_ASSERT_THROWS(m->writeQWord(2, b), WrongSizeInstructionException);
    delete m;
  }

  void testWriteOutOfMemoryHWord(void) {
    ulong size = 2;
    Memory* m = new Memory(size);
    bytes b = bytes {255, 0};
    TS_ASSERT_THROWS(m->writeHWord(1, b), AddressOutOfMemoryException);
    delete m;
  }

  void testWriteOutOfMemoryWord(void) {
    ulong size = 2;
    Memory* m = new Memory(size);
    bytes b = bytes {255, 0, 255, 0};
    TS_ASSERT_THROWS(m->writeWord(1, b), AddressOutOfMemoryException);
    delete m;
  }

  void testWriteOutOfMemoryDWord(void) {
    ulong size = 4;
    Memory* m = new Memory(size);
    bytes b = bytes {255, 0, 255, 0, 255, 0, 255, 0};
    TS_ASSERT_THROWS(m->writeDWord(1, b), AddressOutOfMemoryException);
    delete m;
  }

  void testWriteOutOfMemoryQWord(void) {
    ulong size = 8;
    Memory* m = new Memory(size);
    bytes b = bytes {255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0};
    TS_ASSERT_THROWS(m->writeQWord(1, b), AddressOutOfMemoryException);
    delete m;
  }
};
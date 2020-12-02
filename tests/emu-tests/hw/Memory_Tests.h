#include "../../framework/cxxtest-4.4/cxxtest/TestSuite.h"
#include "../../../src/include/hw/Memory.h"

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
    TS_ASSERT_THROWS_ANYTHING(m->readByte(size));
    delete m;
  }

  void testWriteReadByte(void) {
    ulong size = 128;
    Memory* m = new Memory(size);
    m->writeByte(2, 255);
    TS_ASSERT(m->readByte(2) == 255);
    delete m;
  }

  void testWriteReadWord(void) {
    ulong size = 128;
    Memory* m = new Memory(size);
    bytes b = bytes {255, 0};
    m->writeWord(2, b);
    TS_ASSERT(m->readWord(2) == b);
    delete m;
  }

    void testWriteReadDWord(void) {
     ulong size = 128;
    Memory* m = new Memory(size);
    bytes b = bytes {255, 0, 255, 0};
    m->writeDWord(2, b);
    TS_ASSERT(m->readDWord(2) == b);
    delete m;
  }

  void testWriteReadQWord(void) {
    ulong size = 128;
    Memory* m = new Memory(size);
    bytes b = bytes {255, 0, 255, 0, 255, 0, 255, 0};
    m->writeQWord(2, b);
    TS_ASSERT(m->readQWord(2) == b);
    delete m;
  }

  void testWrongSizeWriteWord(void) {
    ulong size = 128;
    Memory* m = new Memory(size);
    bytes b = bytes {255, 0, 255};
    TS_ASSERT_THROWS_ANYTHING(m->writeWord(2, b));
    delete m;
  }

  void testWrongSizeWriteDWord(void) {
    ulong size = 128;
    Memory* m = new Memory(size);
    bytes b = bytes {255, 0, 255, 0, 255};
    TS_ASSERT_THROWS_ANYTHING(m->writeDWord(2, b));
    delete m;
  }

  void testWrongSizeWriteQWord(void) {
    ulong size = 128;
    Memory* m = new Memory(size);
    bytes b = bytes {255, 0, 255, 0, 255, 0, 255, 0, 255};
    TS_ASSERT_THROWS_ANYTHING(m->writeQWord(2, b));
    delete m;
  }

  void testWriteOutOfMemoryWord(void) {
    ulong size = 2;
    Memory* m = new Memory(size);
    bytes b = bytes {255, 0};
    TS_ASSERT_THROWS_ANYTHING(m->writeWord(1, b));
    delete m;
  }

  void testWriteOutOfMemoryDWord(void) {
    ulong size = 4;
    Memory* m = new Memory(size);
    bytes b = bytes {255, 0, 255, 0};
    TS_ASSERT_THROWS_ANYTHING(m->writeDWord(1, b));
    delete m;
  }

  void testWriteOutOfMemoryQWord(void) {
    ulong size = 8;
    Memory* m = new Memory(size);
    bytes b = bytes {255, 0, 255, 0, 255, 0, 255, 0};
    TS_ASSERT_THROWS_ANYTHING(m->writeQWord(1, b));
    delete m;
  }
};
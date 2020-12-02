#include "include/main.h"
#include "include/bytemanip.h"
#include <string>

int main(int argc, char** argv) {
  Memory* m = new Memory(1024);
  m->writeQWord(24, bytes {255, 0, 254, 0, 253, 0, 252, 0});
  m->printRegion(24, 8, 4);
  ulong size = 8;
  m = new Memory(size);
  bytes b = bytes {255, 0, 255, 0, 255, 0, 255, 0};
  try {
    m->writeQWord(1, b);
  } catch (MemoryException* e) {
    printf(e->getMessage());
  }
  return 0;
}
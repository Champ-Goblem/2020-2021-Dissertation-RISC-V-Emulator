#include "include/main.h"
#include "include/bytemanip.h"
#include "include/instructions/RType.h"
#include <string>

int main(int argc, char** argv) {
    // 0     6 7   1 2 4 5   9 0   4 5     1
    // 0110110 00000 010 11110 11111 1000010
    // 0110 1100 0000 0101 1110 1111 1100 0010
    RTypeInstruction r = RTypeInstruction();
    r.decode(bytes{54, 160, 247, 67});
    printf("%d\n", r.getOpcode());
    printf("%d\n", r.getRD());
    printf("%d\n", r.getFunc3());
    printf("%d\n", r.getRS1());
    printf("%d\n", r.getRS2());
    printf("%d\n", r.getFunc7());
  return 0;
}
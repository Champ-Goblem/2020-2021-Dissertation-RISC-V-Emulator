#include "include/main.h"
#include "include/bytemanip.h"
#include "include/instructions/BType.h"
#include <string>

int main(int argc, char** argv) {
  BTypeInstruction b = BTypeInstruction(127, 1, 15, 7, 31, 31, 63, 1);
  printf("%d\n", b.getImm(8, 11)[0]);
  printf("%d\n", b.getImm(25, 30)[0]);
  printf("%d\n", b.getImm(31, 31)[0]);
  return 0;
}
#include "include/main.h"
#include "include/bytemanip.h"
#include "include/instructions/SType.h"
#include <string>

int main(int argc, char** argv) {
  STypeInstruction s = STypeInstruction(127, 31, 7, 31, 31, 127);
  printf("%d\n", s.getImm(7, 11)[0]);
  printf("%d\n", s.getImm(25, 31)[0]);
  return 0;
}
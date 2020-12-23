#include "include/main.h"
#include "include/bytemanip.h"
#include "include/instructions/JType.h"
#include <string>

int main(int argc, char** argv) {
  bytes imm10 = bytes{255, 3};
  JTypeInstruction j = JTypeInstruction(127, 31, 255, 1, imm10, 1);
  printf("%d\n", j.getOpcode() == 127);
  printf("%d\n", j.getRD() == 31);
  printf("%d\n", j.getImm(12, 19)[0] == 255);
  printf("%d\n", j.getImm(20, 20)[0] == 1);
  printf("%d\n", j.getImm(21, 30) == imm10);
  printf("%d\n", j.getImm(31, 31)[0] == 1);
  return 0;
}
#include "include/main.h"
#include "include/bytemanip.h"
#include "include/instructions/JType.h"
#include <string>

int main(int argc, char** argv) {
  JTypeInstruction j = JTypeInstruction(127, 31, 255, 1, bytes{255, 3}, 1);
  return 0;
}
#include "include/main.h"
#include "include/bytemanip.h"
#include "include/instructions/JType.h"
#include <string>

int main(int argc, char** argv) {
  bytes a = bytes{255, 255};
  addByteToBytes(a, 1);
  return 0;
}
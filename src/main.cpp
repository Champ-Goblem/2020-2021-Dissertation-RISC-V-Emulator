#include "include/main.h"

int main(int argc, char** argv) {
  RegisterFile* rf = new RegisterFile((ushort)4, true);
  bytes val = {255,0,0,0};
  rf->write(1, val);
  rf->debug();
  return 0;
}
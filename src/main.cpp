#include "include/main.h"
#include "include/bytemanip.h"
#include "include/units/SimpleBranchPredictor.h"
#include "include/hw/RegisterFile.h"
#include "include/hw/Memory.h"
#include "include/instructions/SType.h"
#include <thread>

int main(int argc, char** argv) {
  STypeInstruction s = STypeInstruction();
  s.decode(bytes{119, 115, 242, 234});
  s.getImm(25, 31);
  return 0;
}
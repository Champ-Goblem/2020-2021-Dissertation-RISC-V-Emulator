#include "include/main.h"
#include "include/bytemanip.h"
#include "include/units/SimpleBranchPredictor.h"
#include "include/hw/RegisterFile.h"
#include "include/hw/Memory.h"
#include <thread>

int main(int argc, char** argv) {
  Memory m(16);
  RegisterFile rf(4, false);
  try {
    SimpleBranchPredictor s(&m, 4, &rf, bytes{16,0,0,0});
  } catch (EmulatorException* e) {
    cerr << e->getMessage();
  }
  return 0;
}
#include "include/main.h"
#include "include/bytemanip.h"
#include "include/units/SimpleBranchPredictor.h"
#include "include/hw/RegisterFile.h"
#include "include/hw/Memory.h"
#include <thread>

int main(int argc, char** argv) {
    Memory m(20);
    m.writeDWord(0, bytes{115, 0, 64, 1});
    RegisterFile rf(4, false);
    SimpleBranchPredictor s(&m, 4, &rf, bytes{0,0,0,0});
    try {
      s.getNextPC();
    } catch (EmulatorException* e) {
      cerr << e->getMessage();
    }
    this_thread::sleep_for(chrono::seconds(100));
  return 0;
}
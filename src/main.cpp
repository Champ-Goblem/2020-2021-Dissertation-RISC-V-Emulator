#include "include/main.h"
#include "include/bytemanip.h"
#include "include/units/SimpleBranchPredictor.h"
#include "include/hw/RegisterFile.h"
#include "include/hw/Memory.h"
#include <thread>

int main(int argc, char** argv) {
  Memory memory(400);
  // 1100111 00000 000 10000 000100000001
  // 1100 1110 0000 0001 0000 0001 0000 0001
  memory.writeDWord(12, bytes{103, 0, 128, 128});
  RegisterFile rf(4, false);
  rf.write(1, bytes{12, 0, 0, 0});
  SimpleBranchPredictor sb(&memory, 4, &rf, bytes{0,0,0,0});
  cout << getBytesForPrint(sb.getNextPC()) << "\n";
  this_thread::sleep_for(chrono::seconds(1));
  cout << getBytesForPrint(sb.getNextPC()) << "\n";
  this_thread::sleep_for(chrono::seconds(1));
  cout << getBytesForPrint(sb.getNextPC()) << "\n";
  cout << getBytesForPrint(sb.getNextPC()) << "\n";
  cout << getBytesForPrint(sb.getNextPC()) << "\n";
  cout << getBytesForPrint(sb.getNextPC()) << "\n";
  cout << getBytesForPrint(sb.getNextPC()) << "\n";
  cout << getBytesForPrint(sb.getNextPC()) << "\n";
  cout << getBytesForPrint(sb.getNextPC()) << "\n";
  cout << getBytesForPrint(sb.getNextPC()) << "\n";
  cout << getBytesForPrint(sb.getNextPC()) << "\n";
  cout << getBytesForPrint(sb.getNextPC()) << "\n";
  this_thread::sleep_for(chrono::seconds(10));
  cout << getBytesForPrint(sb.getNextPC()) << "\n";
  return 0;
}
#include "include/main.h"
#include "include/bytemanip.h"
#include "include/instructions/AbstractInstruction.h"
#include <string>

class AbstractInstructionTester: public AbstractInstruction {
  public:
  AbstractInstructionTester(bytes imm) {
    this->imm = imm;
  }
};

int main(int argc, char** argv) {
  AbstractInstructionTester i = AbstractInstructionTester(bytes {113, 0, 0, 0});
  bytes ans = i.getImm(0, 6);
  printf(getBytesForPrint(ans).c_str());
  return 0;
}
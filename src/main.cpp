#include "include/main.h"
#include "include/bytemanip.h"
#include "include/instructions/IType.h"
#include <string>

int main(int argc, char** argv) {
  try {
    ITypeInstruction i = ITypeInstruction(127, 31, 7, 31, bytes{255, 15});
  } catch (AbstractInstructionException* e) {
    printf(e->getMessage());
  }
  return 0;
}
#include "../include/units/AbstractBranchPredictor.h"

AbstractBranchPredictor::AbstractBranchPredictor(Memory* memory, ushort XLEN) {
  this->memory = memory;
  this->XLEN = XLEN;
}
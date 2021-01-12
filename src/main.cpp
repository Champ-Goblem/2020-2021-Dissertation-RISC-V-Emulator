#include "include/main.h"
#include "include/bytemanip.h"
#include "include/exceptions.h"
#include "include/units/SimpleBranchPredictor.h"
#include "include/hw/RegisterFile.h"
#include "include/hw/Memory.h"
#include "include/instructions/UType.h"
#include "include/instructions/sets/RV32I.h"
#include <thread>

int main(int argc, char** argv) {
    RV32I base;
    vector<OpcodeSpace> OpS = base.registerOpcodeSpace();
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 23);
    AbstractInstruction inst = decode(bytes{151, 128, 129, 255});
    inst.setPC(bytes{1,12,0,2});
    inst.getType() == InstructionType::U;
    inst.execute(&inst, nullptr, 0);
    RegisterFile rf(4, false);
    inst.registerWriteback(&inst, &rf);
    bytes result{1, 140, 129, 1};
    rf.get(1) == result;
}
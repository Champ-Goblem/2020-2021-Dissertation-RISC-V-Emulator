#include "include/main.h"
#include "include/bytemanip.h"
#include "include/exceptions.h"
#include "include/units/SimpleBranchPredictor.h"
#include "include/hw/RegisterFile.h"
#include "include/hw/Memory.h"
#include "include/instructions/UType.h"
#include "include/instructions/sets/RV32I.h"
#include "include/units/PipelineHazardController.h"

int main(int argc, char** argv) {
    RV32I base;
    vector<OpcodeSpace> OpS = base.registerOpcodeSpace();
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 99);
    bytes instruction{99, 2, 16, 0};
    bytes initialPC{32,0,0,0};
    Memory m(3000);
    m.writeWord(32, instruction);
    RegisterFile rf(4, false);
    rf.write(1, bytes{1, 0, 0, 0});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    bool stall;
    AbstractInstruction inst = decode(instruction, &phc, stall);
    phc.enqueue(&inst);
    inst.setPC(sbp.getNextPC());
    (inst.getType() == InstructionType::B);
    inst.execute(&inst, &sbp, 200, &phc);
    bytes result{36, 0, 0, 0};
    (inst.getResult() == result);
}
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
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 19);
    bytes instruction{147, 112, 129, 120};
    bytes initialPC{4,0,0,0};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(2, bytes{0, 23, 24, 0});
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    AbstractInstruction inst = decode(instruction, &phc);
    inst.setPC(sbp.getNextPC());
    (inst.getType() == InstructionType::I);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{0, 7, 0, 0};
    (inst.getResult() == result);
    inst.registerWriteback(&inst, &rf);
    (rf.get(1) == result);
}
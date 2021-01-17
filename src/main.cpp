#include "include/main.h"
#include "include/bytemanip.h"
#include "include/exceptions.h"
#include "include/units/SimpleBranchPredictor.h"
#include "include/hw/RegisterFile.h"
#include "include/hw/Memory.h"
#include "include/instructions/UType.h"
#include "include/instructions/sets/RV32I.h"
#include "include/units/PipelineHazardController.h"
#include <thread>

int main(int argc, char** argv) {
    RV32I base;
    vector<OpcodeSpace> OpS = base.registerOpcodeSpace();
    Memory m(8000);
    bytes instruction{239, 224, 31, 130};
    bytes initialPC{100, 25, 0, 0}; // 6500
    m.writeWord(6500, instruction);
    RegisterFile rf(4, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 111);
    PipelineHazardController phc(4, &rf, true);
    AbstractInstruction inst = decode(instruction, &phc);
    inst.setPC(sbp.getNextPC());
    (inst.getType() == InstructionType::J);
    inst.execute(&inst, (AbstractBranchPredictor*)&sbp, m.getSize(), &phc);
    inst.registerWriteback(&inst, &rf);
    // res: 0010 0001 1000 0000 0000 0000 0000 0000
    bytes result{132, 1, 0, 0};
    (inst.getResult() == result);
    bytes ret{104, 25, 0, 0};
    (rf.get(1) == ret);
}
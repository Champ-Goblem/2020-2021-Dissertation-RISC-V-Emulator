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
    DecodeRoutine decode = RV32I::findDecodeRoutineByOpcode(OpS, 35);
    bytes instruction{163, 156, 32, 254};
    bytes initialPC{4,0,0,0};
    bytes memoryVal = {2, 183, 32, 0};
    Memory m(3000);
    m.writeWord(4, instruction);
    RegisterFile rf(4, false);
    rf.write(1, bytes{100, 0, 0, 0});
    rf.write(2, memoryVal);
    PipelineHazardController phc(4, &rf, false);
    SimpleBranchPredictor sbp(&m, 4, &rf, initialPC);
    AbstractInstruction inst = decode(instruction, &phc);
    inst.setPC(sbp.getNextPC());
    (inst.getType() == InstructionType::S);
    inst.execute(&inst, &sbp, 3000, &phc);
    bytes result{93, 0, 0, 0};
    (inst.getResult() == result);
    inst.memoryAccess(&inst, &m);
    bytes memVal{2, 183, 0, 0};
    (m.readWord(93) == memVal);
}
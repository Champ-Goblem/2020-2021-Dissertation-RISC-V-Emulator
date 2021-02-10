#include "include/main.h"
#include "include/bytemanip.h"
#include "include/exceptions.h"
#include "include/units/SimpleBranchPredictor.h"
#include "include/hw/RegisterFile.h"
#include "include/hw/Memory.h"
#include "include/instructions/UType.h"
#include "include/instructions/sets/RV32I.h"
#include "include/units/PipelineHazardController.h"
#include "include/hw/Hart.h"

int main(int argc, char** argv) {
    Memory memory = Memory(3000);
    RegisterFile registerFile = RegisterFile(4, false);
    RV32I base = RV32I();
    Hart hart1 = Hart(&memory, &registerFile, base, ExtensionSet(0), 4, bytes{0, 0, 0, 0}, false);
    memory.writeWord(300, bytes{255, 255, 255, 255});
    memory.writeWord(0, bytes{0x93, 0x80, 0xc0, 0x12});
    memory.writeWord(4, bytes{0x03, 0xa1, 0x00, 0x00});
    memory.writeWord(8, bytes{0x93, 0x01, 0xa1, 0x00});
    memory.writeWord(12, bytes{0x33, 0, 0, 0});
    memory.writeWord(16, bytes{0x33, 0, 0, 0});
    memory.writeWord(20, bytes{0x33, 0, 0, 0});
    memory.writeWord(24, bytes{0x33, 0, 0, 0});
    memory.writeWord(28, bytes{0x33, 0, 0, 0});
    memory.writeWord(32, bytes{0x33, 0, 0, 0});
    memory.writeWord(36, bytes{0x33, 0, 0, 0});
    memory.writeWord(40, bytes{0x33, 0, 0, 0});
    try {
        hart1.tick();
        hart1.tick();
        hart1.tick();
        hart1.tick();
        hart1.tick();
        hart1.tick();
        hart1.tick();
        hart1.tick();
        hart1.tick();
        hart1.tick();
        hart1.tick();
    } catch (EmulatorException e) {
        cerr << e.getMessage();
    } catch (exception e) {
        cerr << e.what();
    }
    cout << "";
}
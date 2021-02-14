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

void read(Memory* memory, ulong addr) {
    cout << +memory->readByte(addr) << "\n";
}

int main(int argc, char** argv) {
    Memory memory(3000);
    RegisterFile registerFile1(4, false);
    RegisterFile registerFile2(4, false);
    RV32I base = RV32I();
    memory.writeWord(0, bytes{0x93, 0x80, 0xa0, 0x00});
    memory.writeWord(4, bytes{0x13, 0x01, 0x11, 0x00});
    memory.writeWord(8, bytes{0xe3, 0x9e, 0x20, 0xfe});
    memory.writeWord(12, bytes{0x33, 0x01, 0x00, 0x00});
    memory.writeWord(16, bytes{239, 241, 31, 255});
    memory.writeWord(20, bytes{0x33, 0, 0, 0});
    memory.writeWord(24, bytes{0x33, 0, 0, 0});
    memory.writeWord(28, bytes{0x33, 0, 0, 0});
    memory.writeWord(32, bytes{0x33, 0, 0, 0});
    memory.writeWord(36, bytes{0x33, 0, 0, 0});
    memory.writeWord(40, bytes{0x33, 0, 0, 0});
    Hart hart1(&memory, &registerFile1, base, ExtensionSet(0), 4, bytes{0, 0, 0, 0}, false);
    Hart hart2(&memory, &registerFile2, base, ExtensionSet(0), 4, bytes{0, 0, 0, 0}, false);
    try {
        for (int i=0; i <= 100; i++) {
            // hart1.tick();
            thread hartThread1 = thread(&Hart::tick, &hart1);
            thread hartThread2 = thread(&Hart::tick, &hart2);
            hartThread1.join();
            hartThread2.join();
        }
    } catch (EmulatorException e) {
        cerr << e.getMessage();
    } catch (exception e) {
        cerr << e.what();
    }
    cout << "";
}
# C++ RISC-V Emulator/Simulator

A C++ RISC-V RV32I instruction set simulator with support for extensions and multiple instruction set architectures (ISA)s. This was originally designed and built for a dissertation thesis in simulating RISC-V instruction sets to assist further work in developing a cryptographic hardware extension for RISC-V CPUs.

## The General Design

### HART

The class [Hart](src/hw/Hart.cpp) is the main body of the emulator, and is the namesake of the hardware thread (HART) in the reference material for RISC-V. It is the HARTs job to control the execution of the pipeline and react to any issues that occur during the lifecycle of the program being executed, such as stalls, flushes, failed predictions and halting. The code that performs the execution of each individual instruction is abstracted away from the implementation of the HART and instead deals with the fives stages of the pipeline:

- Fetch
- Decode
- Execute
- Memory Access
- Register Writeback

The method `Hart::tick` controls a single cycle of the HART, progressing an instruction in the pipeline by one stage on each call.

With pipelines comes hazards, and these are detected by the [Pipeline Hazard Controller](src/units/PipelineHazardController.cpp). It records the current instructions in the pipeline and determines if a register value is stale based on the the destination registers of the instructions further into the pipeline. If this is determined to be the case then the pipeline can stall, causing a momentary pause until the value for the source register is calculated. The same class is also used as an intermediary for instructions fetching from registers, and allows forwarding values from instructions that have not yet committed to registers or memory.

### Processor

The class [Processor](src/hw/Processor.cpp) is the controller for the HARTs in a system and performs the setup of each of these and main memory. The main terminal UI (TUI) directly interfaces with this class allowing it to perform pause, stop, step and resume operations on the processor. This class is also responsible for loading the initial flat binary into memory on setup.

### Memory

Main memory is represented by the [Memory](src/hw/Memory.cpp) class, it is simply a one-dimensional array of bytes, stored in a vector. A set of accessors are provided that operate on different lengths of data within memory, the current implementations are:

- HWord -> Half word (2 bytes)
- Word -> Word (4 bytes)
- DWord -> Double Word (8 bytes)
- QWord -> Quad Word (16 bytes)

It also provides some debugging methods to print or get a region of memory determined by a start position and length.

### Register File

The set of registers are contained within the [Register File](src/hw/RegisterFile.cpp) class. This simply holds 16 or 32 registers depending on the underlying architecture, and provides accessors to these through the `get` and `set` methods.

### Branch Prediction

The design of this emulator allows for fully configurable branch prediction, by providing an abstract interface to build off ([AbstractBranchPredictor](src/include/units/AbstractBranchPredictor.h)). This interfaces with each HART through the `handleFlush`, `getNextPC` and `checkPrediction` functions. The `peak` method is used by the TUI to show the next program counter (PC) to the user.

The current implementation supports only ["simple" branch prediction](src/units/SimpleBranchPredictor.cpp), based on the implementation specified in the RISC-V specification. It states that if the sign bit of the immediate of a jumping instruction is negative, then this jump is always taken, otherwise the prediction continues onto the next instruction succeeding the jump. This is analogous to a form of loop in a high level language.

If wanted this can be expanded on in the future to provide more heuristic-based prediction.

### Terminal UI

The rendering of the user interface is controlled by the [Screen](src/screen/screen.cpp) class. This makes use of the [ftxui](https://github.com/ArthurSonzogni/FTXUI) library to display the current program counters in the pipeline, memory output, registers output and a small console.

### Instruction Set Architecture and Instructions

Each instruction once decoded is represented by the [Abstract Instruction](src/include/instructions/AbstractInstruction.h) class. This holds the registers being accessed, fully decoded instruction, the results after execution and the pointers to the methods used to control execution, memory access and writeback. This allows the implementation of each specific instruction to be abstracted out of the HART and instead accessed through the pointers set in `AbstractInstruction::execute`, `AbstractInstruction::registerWriteback` and `AbstractInstruction::memoryAccess` for the corresponding pipeline stages.

A number of instruction types are based upon the abstract implementation mentioned above, and these are:

- [B Type](src/include/instructions/BType.h)
- [I Type](src/include/instructions/IType.h)
- [J Type](src/include/instructions/JType.h)
- [R Type](src/include/instructions/RType.h)
- [S Type](src/include/instructions/SType.h)
- [U Type](src/include/instructions/UType.h)

The meaning for each of these can be found in the reference documentation for RISC-V.

The only currently implemented ISA is [RV32I](src/instructions/sets/RV32I.cpp) or the 32-bit base instruction set. This class holds all of the methods used to perform decoding, execution, memory access and writeback of each instruction where applicable. A map of opcode to decode routine is stored in the `OpcodeSpace` and the subsequent steps in the pipeline are stored within the Instruction class after decoding is performed.

## Design Decisions

Although RV32I is only supported currently, the decision to use `vector<byte>` for all data stored within the program allows the emulator to be extended to support RV64I and even RV128I on currently available 64-bit machines. This meant the usual mathematical operators had to be rewritten to support operations over a number of bytes and these can be found in [bytemanip.h](src/include/bytemanip.h).

## Extending the Instruction Set

Extension of the current instruction set is achieved by defining a new extension or base instruction set alongside the instruction opcodes it supports. The new class must also contain the methods for decoding, execution, memory access and register writeback, which are required to be called by the `tick` function of the HART.

The extension [T](src/instructions/sets/extensions/T.cpp) provides an example for adding extra extensions in the future.

The decode routine is required to initialise the `Instruction` object of an available type. The binary form of the instruction must then be decoded into the fields of the object and the pointers to the corresponding functions set. In the case of data being fetched, this must be performed through the pipeline hazard controller. A check should always be performed for a stale register, returning a No-Operation instruction and setting the parameter `stall` to `true` if this is the case.

For jumps and branches, the calculated address should be verified in the execute stage by calling the `checkPrediction` method of the branch predictor.


## Testing

Tests are implemented in [/tests](/tests) by the cxx-test framework. A set of results for the tests during development are also available in [/tests/results](/tests/results).

<br>
<br>

> *Author: Cameron McDermott (champ_goblem) 2020*
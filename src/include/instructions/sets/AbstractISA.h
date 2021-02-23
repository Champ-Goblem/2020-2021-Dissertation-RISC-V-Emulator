#ifndef __AbstractISA__
#define __AbstractISA__

#include "../../emustd.h"

#include <iterator>
#include <algorithm>

class AbstractInstruction;
class PipelineHazardController;
class AbstractBranchPredictor;
class RegisterFile;
class Memory;

typedef AbstractInstruction (*DecodeRoutine)(bytes, PipelineHazardController*, bool*);
typedef void (*ExecuteRoutine)(AbstractInstruction*, AbstractBranchPredictor*, ulong, PipelineHazardController*);
typedef void (*WritebackRoutine)(AbstractInstruction*, RegisterFile*);
typedef void (*MemoryAccessRoutine)(AbstractInstruction* instruction, Memory* memory, PipelineHazardController*);

enum Bases {
  RV32IBase
};

enum Extensions {};

struct OpcodeSpace {
  ushort opcode;
  DecodeRoutine decodeRoutine;
};

class UndefinedDecodeRoutineException: public EmulatorException {
  public:
  UndefinedDecodeRoutineException(): EmulatorException() {};
  UndefinedDecodeRoutineException(ushort opcode, string message=""): EmulatorException() {
    ostringstream ost;
    ost << "Undefined decode routine for opcode" << "\n";
    ost << "Message: " << message << "\n";
    ost << "\tOpcode: " << opcode << "\n";
    this->message = ost.str();
  };
};

class AbstractISA {
  protected:
  vector<struct OpcodeSpace> opcodeSpace;

  public:
  vector<struct OpcodeSpace> registerOpcodeSpace() { return opcodeSpace; };
  ~AbstractISA() {};

  static DecodeRoutine findDecodeRoutineByOpcode(vector<struct OpcodeSpace> opcodeSpace, ushort opcode) {   
    if (opcodeSpace.size() == 0) {
      throw new EmulatorException("Failed find by opcode, opcode space undefined");
    }

    vector<OpcodeSpace>::iterator itr = find_if(opcodeSpace.begin(), opcodeSpace.end(), findOpcode(opcode));
    if (itr == opcodeSpace.end()) {
      throw UndefinedDecodeRoutineException(opcode, "Check ID stage in pipeline for PC");
    }
    return itr->decodeRoutine;
  }

  private:
  struct findOpcode : std::unary_function<OpcodeSpace, bool> {
    ushort opcode;
    findOpcode(ushort opcode):opcode(opcode) { }
    bool operator()(OpcodeSpace const& os) const {
        return os.opcode == opcode;
    }
};
  
  // Decode routines should be declared private and have a signature of
  // AbstractInstruction decodeSomething(bytes instruction) {...};
};

#endif
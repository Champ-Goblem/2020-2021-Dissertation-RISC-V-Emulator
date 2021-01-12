#ifndef __AbstractISA__
#define __AbstractISA__

#include "../../emustd.h"
#include "../../exceptions.h"
#include "../AbstractInstruction.h"
#include "../BType.h"
#include "../IType.h"
#include "../JType.h"
#include "../RType.h"
#include "../SType.h"
#include "../UType.h"

#include <iterator>
#include <algorithm>

typedef AbstractInstruction (*DecodeRoutine)(bytes);

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
    ost << "Opcode: " << opcode << "\n";
    this->message = ost.str();
  };
};

class AbstractISA {
  public:
  virtual vector<struct OpcodeSpace> registerOpcodeSpace() { return vector<struct OpcodeSpace>(0); };
  static DecodeRoutine findDecodeRoutineByOpcode(vector<struct OpcodeSpace> opcodeSpace, ushort opcode) {
    if (opcodeSpace.size() == 0) {
      throw new EmulatorException("Failed find by opcode, opcode space undefined");
    }

    vector<OpcodeSpace>::iterator itr = find_if(opcodeSpace.begin(), opcodeSpace.end(), findOpcode(opcode));
    if (itr == opcodeSpace.end()) {
      throw UndefinedDecodeRoutineException(opcode);
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
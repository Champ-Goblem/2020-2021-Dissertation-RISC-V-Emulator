#include "include/main.h"
#include "include/bytemanip.h"
#include "include/exceptions.h"
#include "include/hw/Hart.h"
#include "include/screen/screen.h"
#include "include/hw/Processor.h"
#include "include/instructions/sets/RV32I.h"
#include <chrono>
#include <condition_variable>
#include <fstream>

// TODO: Memory output size doesnt respect actual size of memory
// TODO: Fix output looking horrible

std::mutex localMutex;
std::condition_variable conditionVariable;
int memStartAddr = 0, memSize = 128, hartID = 0;
bool stopProcessing, shouldContinue, runningOutput = false, isPaused = false, updateUI = false, isHalted = false;
string extraOutput = "";
EmulatorScreen* screen;
Processor* processor;

int main(int argc, char** argv) {
  if (argc < 2) {
    cout << printArgs() << flush;
    return 1;
  }

  Config config = parseArgs(argc, argv);
  if (config.numberOfHardwareThreads == 0 || config.memorySize == 0 || config.XLEN == 0
    || config.fileLocation == "") {
    cout << printArgs() << flush;
    return 1;
  }

  ifstream file;
  file.open(config.fileLocation);
  if (!file) {
    cout << "Failed to find binary, doesn't exist" << endl;
    cout << "\tPath: " << config.fileLocation << endl;
    return 1;
  }

  shouldContinue = !config.pauseOnEntry;

  processor = new Processor(config);

  // Defines the button names and callback functions
  vector<ButtonMetadata> buttonMetadata = {
    // ButtonMetadata{
    //   .text = "Start",
    //   .fn = std::bind<>(&start)
    // },
    ButtonMetadata{
      .text = "Stop",
      .fn = std::bind<>(&stop)
    },
    ButtonMetadata{
      .text = "Pause",
      .fn = std::bind<>(&pause)
    },
    ButtonMetadata{
      .text = "Resume",
      .fn = std::bind<>(&resume)
    },
    ButtonMetadata{
      .text = "Step",
      .fn = std::bind<>(&step)
    }
  };

  screen = new EmulatorScreen(config.XLEN, buttonMetadata, parseInputContent);

  // Start the workloop for the program
  while (!stopProcessing) {
    if (!shouldContinue) {
      renderUI(screen, processor, "Paused");
      isPaused = true;
      // In order to pause the main thread to stop it executing
      // we use ConditionVariable::wait, this condition variable
      // is signaled to start executing once a user clicks 
      // resume
      unique_lock<mutex> lckGuard(localMutex);
      conditionVariable.wait(lckGuard);
      renderUI(screen, processor, "Running...");
      isPaused = false;
    } else if (runningOutput || updateUI) {
      renderUI(screen, processor, "Running...");
      if (updateUI) updateUI = false;
    }

    try {
      processor->step();
    } catch (HaltedProcessor) {
      renderUI(screen, processor, "Halted\nOnly 'Stop' and commands will work");
      stopProcessing = true;
      isHalted = true;
    } catch (EmulatorException e) {
      renderUI(screen, processor, e.getMessage() + "\nExiting...");
      stopProcessing = true;
    } catch (exception e) {
      renderUI(screen, processor, string(e.what()) + "\nExiting...");
      stopProcessing = true;
    }
  }

  if (isHalted) {
    isPaused = true;
    unique_lock<mutex> lckGuard(localMutex);
    conditionVariable.wait(lckGuard);
  }
  

  delete(processor);
}

string printArgs() {
  ostringstream oss;
  oss << "RISC-V Emulator Arguments:\n";
  oss << "\t--base-isa= -> Select the base ISA to choose from, selection: [RV32I]\n";
  oss << "\t--extensions= -> The set of extensions to use, selection: []\n";
  oss << "\t--branch-predictor= -> The choice of branch predictor to use: [Simple]\n";
  oss << "\t--memory-size= -> Number of bytes to allocate for memory\n";
  oss << "\t--hardware-threads= -> Number of hardware threads to use\n";
  oss << "\t--binary= -> Flat binary to load into memory\n";
  oss << "\t--pause -> Pause on entry\n";
  oss << "\t--runningOutput -> Update the UI while the processor is running (Will slow down normal execution)\n";
  oss << "\t--halt=<hex> -> Specify halt address or detect it from the final address of the binary\n";
  oss << "\t-h --help -> this help screen\n";
  return oss.str();
}

string stringSplit(string arg, char delim) {
  // Performs a simple split string on the first instance of the delimiter
  int pos = arg.find(delim);
  if (pos != std::string::npos) {
    return arg.substr(pos+1, arg.size() - 1);
  }

  return "";
}

Config parseArgs(int argc, char** argv) {
  // Parse commandline arguments and form out basic config
  // struct to pass to the processor
  Config config;

  for (uint i=1; i < argc; i++) {
    string arg(argv[i]);
    if (arg.find("base-isa") != std::string::npos) {
      // Set base isa
      string param = stringSplit(arg, '=');
      if (param == "RV32I") {
        config.baseISA = Bases::RV32IBase;
        config.XLEN = 4;
      }

    } else if (arg.find("extensions") != std::string::npos) {
      // Set extensions
      string param = stringSplit(arg, '=');
      // TODO: Needs updated when extensions added

    } else if (arg.find("memory-size") != std::string::npos) {
      // Set the memory size
      string param = stringSplit(arg, '=');
      if (param != "") {
        try {
          ulong size = stol(param);
          config.memorySize = size;
        } catch (exception e ) {
          cout << "Failed to set memory size" << endl;
          config.memorySize = 0;
          config.numberOfHardwareThreads = 0;
          return config;
        }
      }

    } else if (arg.find("hardware-threads") != std::string::npos) {
      // Set the number of hardware threads
      string param = stringSplit(arg, '=');
      if (param != "") {
        try {
          ulong no = stol(param);
          config.numberOfHardwareThreads = no;
        } catch (exception e ) {
          cout << "Failed to set hardware threads count" << endl;
          config.memorySize = 0;
          config.numberOfHardwareThreads = 0;
          return config;
        }
      }

    } else if (arg.find("branch-predictor") != std::string::npos) {
      // Set the branch predictor used
      string param = stringSplit(arg, '=');
      if (param == "Simple") {
        config.branchPredictor = BranchPredictors::Simple;
      }
    } else if (arg.find("binary") != std::string::npos) {
      // Get the location of the binary to load into memory
      config.fileLocation = stringSplit(arg, '=');

    } else if (arg.find("pause") != std::string::npos) {
      // Set if the emulator starts off paused
      config.pauseOnEntry = true;

    } else if (arg.find("runningOutput") != string::npos) {
      // Set if output runs constantly when the simulator is running
      runningOutput = true;

    } else if (arg.find("halt") != string::npos) {
      // Set the halt related stuff
      string param = stringSplit(arg, '=');
      if (param != "") {
        if (param.length() % 2 != 0) {
          cout << "Halt address not hex encoded" << endl;
          config.memorySize = 0;
          config.numberOfHardwareThreads = 0;
          return config;
        }
        if (param.length() > 8) {
          cout << "Halt address longer than 8 bytes" << endl;
          config.memorySize = 0;
          config.numberOfHardwareThreads = 0;
          return config;
        }
        bytes haltAddr(8);
        for (uint i=0; i < param.length(); i+=2) {
          stringstream oss;
          uint out;
          oss << hex << param.substr(i, i+2);
          oss >> out;
          haltAddr[floor((float)i/2)] = (byte)out;
        }
        config.haltAddr = haltAddr;
      }

    } else if (arg.find("help") != std::string::npos || arg.find("h") != std::string::npos) {
      // Return an invalid config so that commands output is displayed
      config.memorySize = 0;
      config.numberOfHardwareThreads = 0;
      return config;
    } else {
      config.memorySize = 0;
      config.numberOfHardwareThreads = 0;
      return config;
    }
  }

  return config;
}

// void start() {
//   // Start the simulator
//   shouldContinue = true;
//   localMutex.unlock();
//   conditionVariable.notify_all();
// }

void stop() {
  // Stop the simulator
  stopProcessing = true;
  localMutex.unlock();
  conditionVariable.notify_all(); 
}

void pause() {
  if (!isHalted) {
    // Pause the simulator
    localMutex.unlock();
    shouldContinue = false;
  }
}

void resume() {
  if (!isHalted) {
    // Resume the simulator
    shouldContinue = true;
    localMutex.unlock();
    conditionVariable.notify_all();
  }
}

void step() {
  if (!isHalted) {
    // Tick the simulator for one cycle
    localMutex.unlock();
    conditionVariable.notify_all();
  }
}

void renderUI(EmulatorScreen* screen, Processor* processor, string output, bool stopRenderThread) {
  // Get all necessary debug information then call the render utility
  vector<bytes> pipeline(0);
  vector<bytes> registers(0);
  bytes memory(0);
  try {
    pipeline = processor->debug(GET_PIPELINE, hartID);
    registers = processor->debug(GET_REGISTERS, hartID);
  } catch (FailedDebugException e) {
    hartID = 0;
    pipeline = processor->debug(GET_PIPELINE, 0);
    registers = processor->debug(GET_REGISTERS, 0);
    extraOutput += e.getMessage() + "\n";
  }

  try {
    memory = processor->getMemoryRegion(memStartAddr, memSize);
  } catch (AddressOutOfMemoryException e) {
    memStartAddr = 0;
    memSize = 128;
    memory = processor->getMemoryRegion(0, 128);
    extraOutput += e.getMessage() + "\n";
  }
  screen->render(pipeline, registers, memory, extraOutput + output, memStartAddr, hartID, stopRenderThread);
  extraOutput = "";
}

void parseInputContent(string content) {
  if (content.find("help") != string::npos) {
    ostringstream oss;
    oss << "Commands:" << endl;
    oss << "memW <int> -> Set memory output width" << endl;
    oss << "memA <int> -> Set memory start addr" << endl;
    oss << "hID <int> -> Set select hart via ID" << endl;
    oss << "hList -> List the hart IDs available" << endl;
    oss << "flush -> Flush the pipeline" << endl;
    oss << "upd -> One time UI update" << endl;
    extraOutput = oss.str();
  } else if (content.find("memW") != string::npos) {
    // Set the number of bytes that are output for
    // the memory region
    string param = stringSplit(content, ' ');
    if (param != "") {
      try {
        memSize = stol(param);
      } catch (exception e) {
        extraOutput = "Failed to set memW\n";
      }
    } else {
      extraOutput = "Faileds to set memW\n";
    }
  } else if (content.find("memA") != string::npos) {
    // Set the start address for the memory
    string param = stringSplit(content, ' ');
    if (param != "") {
      try {
        memStartAddr = stol(param);
      } catch (exception e) {
        extraOutput = "Failed to set memA\n";
      }
    } else {
      extraOutput = "Failed to set memA\n";
    }

  } else if (content.find("hID") != string::npos) {
    // Set the hart via its ID
    string param = stringSplit(content, ' ');
    if (param != "") {
      try {
        hartID = stol(param);
      } catch (exception e) {
        extraOutput = "Failed to set hID\n";
      }
    } else {
      extraOutput = "Failed to set hID\n";
    }

  } else if (content.find("hList") != string::npos) {
    // List all the harts available to the process
    uint noHARTs = processor->getNumberOfHarts() - 1;
    if (noHARTs > 0) {
      extraOutput = "HartIDs: 0 -> " + to_string(noHARTs) + "\n";
    } else {
      extraOutput = "HartIDs: 0\n";
    }

  } else if (content.find("flush") != string::npos) {
    processor->flush();

  } else if (content.find("upd") != string::npos) {
    updateUI = true;

  } else {
    extraOutput = "Unknown command, 'help' for help\n";
  }

  if (isPaused) {
    // If processing is paused then we need to re-render the UI
    // so that the output is update
    renderUI(screen, processor, isHalted ? "\nHalted" : "\nPaused", false);
  } else {
    updateUI  = true;
  }
}
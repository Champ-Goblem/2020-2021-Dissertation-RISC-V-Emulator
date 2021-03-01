#include "include/main.h"
#include "include/exceptions.h"
#include "include/hw/Hart.h"
#include "include/screen/screen.h"
#include "include/hw/Processor.h"
#include "include/instructions/sets/RV32I.h"
#include <chrono>
#include <condition_variable>
#include <fstream>

std::mutex localMutex;
std::condition_variable conditionVariable;
int memStartAddr = 0, memSize = 128, hartID = 0;
bool stopProcessing, shouldContinue, runningOutput = false, isPaused = false;
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
    cout << "Failed to find binary, doesnt exist" << endl;
    cout << "\tPath: " << config.fileLocation << endl;
    return 1;
  }

  shouldContinue = !config.pauseOnEntry;

  processor = new Processor(config);
  vector<ButtonMetadata> buttonMetadata = {
    ButtonMetadata{
      .text = "Start",
      .fn = std::bind<>(&start)
    },
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
  // ulong count;
  // chrono::steady_clock sc;
  // chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
  while (!stopProcessing) {
    if (!shouldContinue) {
      renderUI(screen, processor, "Paused");
      isPaused = true;
      unique_lock<mutex> lckGuard(localMutex);
      conditionVariable.wait(lckGuard);
      isPaused = false;
    } else if (runningOutput) {
      renderUI(screen, processor, "Running...");
    }

    try {
      processor->step();
    } catch (EmulatorException e) {
      renderUI(screen, processor, e.getMessage() + "\nExiting...");
      stopProcessing = true;
    } catch (exception e) {
      renderUI(screen, processor, string(e.what()) + "\nExiting...");
      stopProcessing = true;
    }
    // count++;
    // const double uptime = (std::chrono::steady_clock::now() - start).count();
    // cout << uptime << "\n";
    // cout << count / uptime << "Ops\n";
  }


  delete(screen);
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
  oss << "\t-h --help -> this help screen\n";
  return oss.str();
}

string stringSplit(string arg, char delim) {
  int pos = arg.find(delim);
  if (pos != std::string::npos) {
    return arg.substr(pos+1, arg.size() - 1);
  }

  return "";
}

Config parseArgs(int argc, char** argv) {
  Config config;

  for (uint i=1; i < argc; i++) {
    string arg(argv[i]);
    if (arg.find("base-isa") != std::string::npos) {
      string param = stringSplit(arg, '=');
      if (param == "RV32I") {
        config.baseISA = Bases::RV32IBase;
        config.XLEN = 4;
      }

    } else if (arg.find("extensions") != std::string::npos) {
      string param = stringSplit(arg, '=');
      // TODO: Needs adding when extensions added

    } else if (arg.find("memory-size") != std::string::npos) {
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
      string param = stringSplit(arg, '=');
      if (param == "Simple") {
        config.branchPredictor = BranchPredictors::Simple;
      }
    } else if (arg.find("binary") != std::string::npos) {
      config.fileLocation = stringSplit(arg, '=');
    } else if (arg.find("pause") != std::string::npos) {
      config.pauseOnEntry = true;
    } else if (arg.find("runningOutput") != string::npos) {
      runningOutput = true;
    } else if (arg.find("help") != std::string::npos || arg.find("h") != std::string::npos) {
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

void start() {
  cout << "start" << endl;
  shouldContinue = true;
  localMutex.unlock();
  conditionVariable.notify_all();
}

void stop() {
  cout << "stop" << endl; 
  stopProcessing = true;
  localMutex.unlock();
  conditionVariable.notify_all(); 
}

void pause() {
  cout << "pause" << endl;
  localMutex.unlock();
  shouldContinue = false;
}

void resume() {
  cout << "resume" << endl;
  shouldContinue = true;
  localMutex.unlock();
  conditionVariable.notify_all();
}

void step() {
  cout << "step" << endl;
  localMutex.unlock();
  conditionVariable.notify_all();
}

void renderUI(EmulatorScreen* screen, Processor* processor, string output, bool stopRenderThread) {
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
    extraOutput = oss.str();
  } else if (content.find("memW") != string::npos) {
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
    uint noHARTs = processor->getNumberOfHarts() - 1;
    if (noHARTs > 0) {
      extraOutput = "HartIDs: 0 -> " + to_string(noHARTs) + "\n";
    } else {
      extraOutput = "HartIDs: 0\n";
    }

  } else {
    extraOutput = "Unknown command, 'help' for help\n";
  }

  if (isPaused) {
    renderUI(screen, processor, "Paused\n", false);
  }
}
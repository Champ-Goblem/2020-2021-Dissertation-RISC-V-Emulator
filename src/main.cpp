#include "include/main.h"
#include "include/hw/Hart.h"
#include "include/screen/screen.h"
#include "include/hw/Processor.h"
#include "include/instructions/sets/RV32I.h"
#include <condition_variable>
#include <fstream>

std::mutex localMutex;
std::condition_variable conditionVariable;
bool stopProcessing, shouldContinue;

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

  Processor processor(config);
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

  EmulatorScreen screen(config.XLEN, buttonMetadata);
  while (!stopProcessing) {
    if (!shouldContinue) {
      renderUI(&screen, &processor, "Paused");
      unique_lock<mutex> lckGuard(localMutex);
      conditionVariable.wait(lckGuard);
    }

    try {
      processor.step();
    } catch (EmulatorException e) {
      renderUI(&screen, &processor, e.getMessage() + "\nExiting...");
      stopProcessing = true;
    } catch (exception e) {
      renderUI(&screen, &processor, string(e.what()) + "\nExiting...");
      stopProcessing = true;
    }
  }

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
  oss << "\t-h --help -> this help screen\n";
  return oss.str();
}

string getArgParameter(string arg) {
  int pos = arg.find('=');
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
      string param = getArgParameter(arg);
      if (param == "RV32I") {
        config.baseISA = Bases::RV32IBase;
        config.XLEN = 4;
      }

    } else if (arg.find("extensions") != std::string::npos) {
      string param = getArgParameter(arg);
      // TODO: Needs adding when extensions added

    } else if (arg.find("memory-size") != std::string::npos) {
      string param = getArgParameter(arg);
      if (param != "") {
        ulong size = stol(param);
        config.memorySize = size;
      }

    } else if (arg.find("hardware-threads") != std::string::npos) {
      string param = getArgParameter(arg);
      if (param != "") {
        ulong no = stol(param);
        config.numberOfHardwareThreads = no;
      }

    } else if (arg.find("branch-predictor") != std::string::npos) {
      string param = getArgParameter(arg);
      if (param == "Simple") {
        config.branchPredictor = BranchPredictors::Simple;
      }
    } else if (arg.find("binary") != std::string::npos) {
      config.fileLocation = getArgParameter(arg);
    } else if (arg.find("pause") != std::string::npos) {
      config.pauseOnEntry = true;
    } else if (arg.find("help") != std::string::npos || arg.find("h") != std::string::npos) {
      config.memorySize = 0;
      config.numberOfHardwareThreads = 0;
    } else {
      config.memorySize = 0;
      config.numberOfHardwareThreads = 0;
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

void renderUI(EmulatorScreen* screen, Processor* processor, string output) {
  vector<bytes> pipeline = processor->debug(GET_PIPELINE, 0);
  vector<bytes> registers = processor->debug(GET_REGISTERS, 0);
  bytes memory = processor->getMemoryRegion(0, 128);
  screen->render(pipeline, registers, memory, output, 0);
}

// Memory memory(3000);
// RV32I base = RV32I();
// memory.writeWord(0, bytes{0x93, 0x80, 0xa0, 0x00});
// memory.writeWord(4, bytes{0x13, 0x01, 0x11, 0x00});
// memory.writeWord(8, bytes{0xe3, 0x9e, 0x20, 0xfe});
// memory.writeWord(12, bytes{0x33, 0x01, 0x00, 0x00});
// memory.writeWord(16, bytes{239, 241, 31, 255});
// memory.writeWord(20, bytes{0x33, 0, 0, 0});
// memory.writeWord(24, bytes{0x33, 0, 0, 0});
// memory.writeWord(28, bytes{0x33, 0, 0, 0});
// memory.writeWord(32, bytes{0x33, 0, 0, 0});
// memory.writeWord(36, bytes{0x33, 0, 0, 0});
// memory.writeWord(40, bytes{0x33, 0, 0, 0});
// Hart hart1(&memory, base, ExtensionSet(0), 4, bytes{0, 0, 0, 0}, false);
// Hart hart2(&memory, base, ExtensionSet(0), 4, bytes{0, 0, 0, 0}, false);
// try {
//     for (int i=0; i <= 100; i++) {
//         exception_ptr h1, h2;
//         // hart1.tick();
//         thread hartThread1 = thread(&Hart::tick, &hart1, h1);
//         thread hartThread2 = thread(&Hart::tick, &hart2, h2);
//         hartThread1.join();
//         hartThread2.join();
//     }
// } catch (EmulatorException e) {
//     cerr << e.getMessage();  
// } catch (exception e) {
//     cerr << e.what();
// }
// cout << "";
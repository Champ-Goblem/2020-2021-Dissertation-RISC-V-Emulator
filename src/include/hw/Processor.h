#ifndef __Processor__
#define __Processor__

#include "../emustd.h"
#include "../hw/Hart.h"
#include "../hw/Memory.h"
struct Config {
  Bases baseISA;
  vector<Extensions> extensionSet;
  BranchPredictors branchPredictor;
  bool isRV32E;
  ushort numberOfHardwareThreads;
  ulong memorySize;
  ushort XLEN;
  bool pauseOnEntry;
  string fileLocation;
  Config() {
    extensionSet = vector<Extensions>(0);
    isRV32E = false;
    numberOfHardwareThreads = 0;
    memorySize = 0;
    XLEN = 0;
    pauseOnEntry = false;
    fileLocation = "";
  }
};

class Processor {
  private:
  Memory memory;
  vector<Hart*> hardwareThreads;
  Config config;
  bool shouldContinue = true;

  public:
  Processor(Config config);
  ~Processor();
  void loadFile(string filepath);
  void start();
  void stop();
  void pause();
  void resume();
  void step();
  vector<bytes> debug(DEBUG debug, uint hartID);
  bytes getMemoryRegion(ulong start, ulong count);
  uint getNumberOfHarts() {
    return config.numberOfHardwareThreads;
  }

  private:
  void runStep();
};

#endif
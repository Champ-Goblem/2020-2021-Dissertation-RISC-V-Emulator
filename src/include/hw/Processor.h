#ifndef __Processor__
#define __Processor__

#include "../emustd.h"
#include "../hw/Hart.h"
#include "../hw/Memory.h"

struct Config {
  AbstractISA baseISA;
  ExtensionSet extensionSet;
  bool isRV32E;
  ushort numberOfHardwareThreads;
  ulong memorySize;
  ushort XLEN;
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
  string debug();

  private:
  void runStep();
};

#endif
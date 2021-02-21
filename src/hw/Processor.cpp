  renderUI(&screen, &processor);
#include "../include/hw/Processor.h"
#include <exception>
#include <fstream>
#include "../include/exceptions.h"

Processor::Processor(Config config): memory(config.memorySize) {
  this->config = config;
  this->hardwareThreads= vector<Hart*>(config.numberOfHardwareThreads);
  for (uint i=0; i < config.numberOfHardwareThreads; i++) {
    this->hardwareThreads[i] = new Hart(&this->memory, config.baseISA, config.extensionSet, config.branchPredictor, config.XLEN, bytes{0, 0, 0, 0}, config.isRV32E);
  }
}

Processor::~Processor() {
  for (uint i=0; i < this->config.numberOfHardwareThreads; i++) {
    delete(this->hardwareThreads[i]);
  }
}

void Processor::start() {
  while (this->shouldContinue) {
    runStep();
  }
}

void Processor::pause() {
  this->shouldContinue = false;
}

void Processor::resume() {
  this->shouldContinue = true;
  start();
}

void Processor::step() {
  runStep();
}

void Processor::runStep() {
  vector<thread> threads;
    vector<exception_ptr> exceptions(config.numberOfHardwareThreads);
    for (uint i=0; i < this->config.numberOfHardwareThreads; i++) {
      threads.push_back(thread(&Hart::tick, hardwareThreads[i], ref(exceptions[i])));
    }

    for (uint i=0; i < this->config.numberOfHardwareThreads; i++) {
      threads[i].join();

      if (exceptions[i]) {
        rethrow_exception(exceptions[i]);
      }
    }
}

void Processor::loadFile(string filePath) {
  ifstream file;
  file.open(filePath);
  if (!file) {
    throw FailedToLoadProgramException(filePath, "Binary does not exist");
  }
  ulong addr=0;
  while (!file.eof()) {
    if (addr > memory.getSize()) {
      throw FailedToLoadProgramException(filePath, "Binary larger than memory");
    }
    unsigned char c;
    file >> c;
    memory.writeByte(c, addr);
    addr++;
  }

  file.close();
}

vector<bytes> Processor::debug(DEBUG debug, uint hartID) {
  if (hartID > hardwareThreads.size()) {
    throw FailedDebugException("Failed to access hart, doesn't exist");
  }
  return hardwareThreads[hartID]->debug(debug);
}

bytes Processor::getMemoryRegion(ulong start, ulong count) {
  return memory.getRegion(start, count);
}
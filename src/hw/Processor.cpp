#include "../include/hw/Processor.h"
#include <exception>
#include <fstream>
#include "../include/exceptions.h"

Processor::Processor(Config config): memory(config.memorySize) {
  this->config = config;
  this->hardwareThreads= vector<Hart*>(config.numberOfHardwareThreads);
  if (this->config.haltAddr.size() > 0) {
    this->config.haltAddr.resize(config.XLEN);
  }
  for (uint i=0; i < config.numberOfHardwareThreads; i++) {
    this->hardwareThreads[i] = new Hart(&this->memory, config.baseISA, config.extensionSet, config.branchPredictor, config.XLEN, bytes(config.XLEN), config.isRV32E, this->config.haltAddr);
  }
  loadFile(config.fileLocation);
}

Processor::~Processor() {
  for (uint i=0; i < this->config.numberOfHardwareThreads; i++) {
    delete(this->hardwareThreads[i]);
  }
}

void Processor::start() {
  // Workloop for the processor
  while (this->shouldContinue) {
    runStep();
  }
}

void Processor::pause() {
  // Pause the processor
  this->shouldContinue = false;
}

void Processor::resume() {
  // Resumes the processor
  this->shouldContinue = true;
  start();
}

void Processor::step() {
  // Public
  runStep();
}

void Processor::runStep() {
  // Runs a single step of the processor
  // Uses a vector of threads, size of number of cores
  // creating a thread for each core so they execute in parallel
  vector<thread> threads;
  vector<exception_ptr> exceptions(config.numberOfHardwareThreads);
  for (uint i=0; i < this->config.numberOfHardwareThreads; i++) {
    threads.push_back(thread(&Hart::tick, hardwareThreads[i], ref(exceptions[i])));
  }

  for (uint i=0; i < this->config.numberOfHardwareThreads; i++) {
    threads[i].join();
  }

  for (uint i=0; i < this->config.numberOfHardwareThreads; i++) {
    if (exceptions[i]) {
      rethrow_exception(exceptions[i]);
    }
  }
}

void Processor::flush() {
  // Flush the pipelines of all the harts
  vector<thread> threads;
  vector<exception_ptr> exceptions(config.numberOfHardwareThreads);
  for (uint i=0; i < this->config.numberOfHardwareThreads; i++) {
    threads.push_back(thread(&Hart::flush, hardwareThreads[i], ref(exceptions[i])));
  }

  for (uint i=0; i < this->config.numberOfHardwareThreads; i++) {
    threads[i].join();

    if (exceptions[i]) {
      rethrow_exception(exceptions[i]);
    }
  }
}

void Processor::loadFile(string filePath) {
  // Load the file into memory byte by byte
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
    unsigned char c = file.get();
    // file >> c;
    // cout << hex << (uint)c << endl;
    memory.writeByte(addr, c);
    addr++;
  }

  file.close();
}

vector<bytes> Processor::debug(DEBUG debug, uint hartID) {
  // Used to debug the emulator
  if (hartID > hardwareThreads.size() - 1) {
    throw FailedDebugException("Hart doesn't exist");
  }
  return hardwareThreads[hartID]->debug(debug);
}

bytes Processor::getMemoryRegion(ulong start, ulong count) {
  // Returns the memory region between start, start+count
  return memory.getRegion(start, count);
}
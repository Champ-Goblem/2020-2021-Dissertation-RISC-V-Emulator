#include "../include/hw/Processor.h"
#include <exception>

Processor::Processor(Config config): memory(config.memorySize) {
  this->config = config;
  this->hardwareThreads= vector<Hart*>(config.numberOfHardwareThreads);
  for (uint i=0; i < config.numberOfHardwareThreads; i++) {
    this->hardwareThreads[i] = new Hart(&this->memory, config.baseISA, config.extensionSet, config.XLEN, bytes{0, 0, 0, 0}, config.isRV32E);
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
    vector<exception_ptr> exceptions;
    for (uint i=0; i < this->config.numberOfHardwareThreads; i++) {
      threads[i] = thread(&Hart::tick, hardwareThreads[i], exceptions[i]);
    }

    for (uint i=0; i < this->config.numberOfHardwareThreads; i++) {
      threads[i].join();

      if (exceptions[i]) {
        try {
          rethrow_exception(exceptions[i]);
        } catch (EmulatorException e) {
          // TODO: Output the error to the user
        } catch (exception e) {
        }
      }
    }
}
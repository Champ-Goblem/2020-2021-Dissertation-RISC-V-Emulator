#ifndef __emustd__
#define __emustd__

#include <stdio.h>
#include <vector>
#include <cmath>
#include <math.h>
#include <thread>

using namespace std;

typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef vector<byte> bytes;

class EmulatorException: public exception {
  private:
  const char* message;

  public:
  EmulatorException() {}
  EmulatorException(const char* message, ...) {
    this->message = message;
  }
  const char* getMessage() {
    return message;
  }
};

#endif
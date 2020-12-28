#ifndef __emustd__
#define __emustd__

#include <stdio.h>
#include <vector>
#include <cmath>
#include <math.h>
#include <thread>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef vector<byte> bytes;

class EmulatorException: public exception {
  protected:
  string message;

  public:
  EmulatorException() {}
  EmulatorException(string message) {
    this->message = message;
  }
  string getMessage() {
    return message;
  }
};

#endif
#ifndef __AbstractUnit__
#define __AbstractUnit__

#include "../emustd.h"

class AbstractUnit {
  protected:
  bool isProcessorExceptionGenerated = false;

  public:
  void processorExceptionGenerated();
};

#endif
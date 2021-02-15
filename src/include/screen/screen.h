#ifndef __EmulatorScreen__
#define __EmulatorScreen__

#include "../emustd.h"

#include "../../lib/headers/ftxui/dom/elements.hpp"
#include "../../lib/headers/ftxui/screen/screen.hpp"

using namespace ftxui;

#define PIPELINE_LOOKBACK_COUNT 3

class EmulatorScreen {
  private:
  string resetPosition;
  vector<vector<bytes>> previousPipelineStages;
  ushort XLEN;

  public:
  EmulatorScreen(ushort XLEN);
  Element renderPipeline(vector<bytes> stages);
  Element renderMemory(vector<bytes> segment);
  Element renderSTDOut(string message);
  Element renderRegisterFile(vector<bytes> registerValues);
  void render(vector<bytes> pipelineStage, vector<bytes> registerValues, vector<bytes> memorySegment, string stdoutMessage);
};

#endif
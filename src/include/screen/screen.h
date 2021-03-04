#ifndef __EmulatorScreen__
#define __EmulatorScreen__

#include "../emustd.h"

#include "../../lib/headers/ftxui/dom/elements.hpp"
#include "../../lib/headers/ftxui/component/button.hpp"
#include "../../lib/headers/ftxui/screen/screen.hpp"
#include "../../lib/headers/ftxui/component/container.hpp"
#include "../../lib/headers/ftxui/component/input.hpp"
#include "../../include/hw/Processor.h"
#include "../../lib/headers/ftxui/component/screen_interactive.hpp"

using namespace ftxui;

#define PIPELINE_LOOKBACK_COUNT 3

typedef void (Processor::*ButtonFn)();

struct ButtonMetadata {
  string text;
  std::function<void()> fn;
};

class InteractiveComponent: public Component {
  private:
  vector<Button*> buttons;
  Container container = Container::Horizontal();
  Input* inputBox;
  function<void(string)> inputOnEnterCallback;

  public:
  InteractiveComponent(vector<ButtonMetadata> buttonMetadata, function<void(string)> inputOnEnterCallback);
  ~InteractiveComponent();
  
  private:
  void handleInputOnEnter();
};

class EmulatorScreen {
  private:
  string resetPosition;
  vector<vector<bytes>> previousPipelineStages;
  ushort XLEN;
  vector<Elements> previousMessages;
  thread interactiveController;
  ScreenInteractive* screeni;
  uint width, registerWidth;

  InteractiveComponent interactiveComponent;

  public:
  EmulatorScreen(ushort XLEN, vector<ButtonMetadata> buttonMetadata, function<void(string)> inputOnEnterCallback);
  ~EmulatorScreen();
  void render(vector<bytes> pipelineStage, vector<bytes> registerValues, vector<byte> memorySegment, string stdoutMessage, ulong startAddr,
    uint selectedHartID, bool stopRenderThread=true);

  private:
  Element renderPipeline(vector<bytes> stages);
  Element renderMemory(vector<byte> segment, ulong addr);
  Element renderSTDOut(string message, uint selectedHartID);
  Element renderRegisterFile(vector<bytes> registerValues);
};

#endif
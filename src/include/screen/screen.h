#ifndef __EmulatorScreen__
#define __EmulatorScreen__

#include "../emustd.h"

#include "../../lib/headers/ftxui/dom/elements.hpp"
#include "../../lib/headers/ftxui/component/button.hpp"
#include "../../lib/headers/ftxui/screen/screen.hpp"
#include "../../lib/headers/ftxui/component/container.hpp"
#include "../../include/hw/Processor.h"
#include "../../lib/headers/ftxui/component/screen_interactive.hpp"

using namespace ftxui;

#define PIPELINE_LOOKBACK_COUNT 3
#define MEMORY_OUTPUT_WIDTH 16

typedef void (Processor::*ButtonFn)();

struct ButtonMetadata {
  string text;
  std::function<void()> fn;
};

class ButtonComponent: public Component {
  private:
  vector<Button*> buttons;
  Container container = Container::Horizontal();

  public:
  ButtonComponent(vector<ButtonMetadata> buttonMetadata);
  ~ButtonComponent();
};

class EmulatorScreen {
  private:
  string resetPosition;
  vector<vector<bytes>> previousPipelineStages;
  ushort XLEN;
  vector<Elements> previousMessages;
  thread buttonController;
  ScreenInteractive* screeni;

  ButtonComponent buttonComponent;

  public:
  EmulatorScreen(ushort XLEN, vector<ButtonMetadata> buttonMetadata);
  ~EmulatorScreen();
  Element renderPipeline(vector<bytes> stages);
  Element renderMemory(vector<byte> segment, ulong addr);
  Element renderSTDOut(string message);
  Element renderRegisterFile(vector<bytes> registerValues);
  Element renderButtons();
  void render(vector<bytes> pipelineStage, vector<bytes> registerValues, vector<byte> memorySegment, string stdoutMessage, ulong startAddr);
};

#endif
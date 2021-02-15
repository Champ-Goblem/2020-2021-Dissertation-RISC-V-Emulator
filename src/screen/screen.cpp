#include "../include/screen/screen.h"
#include "../include/bytemanip.h"
#include "../include/screen/helpers.h"

EmulatorScreen::EmulatorScreen(ushort XLEN) {
  this->XLEN = XLEN;

  for (uint i=0; i < PIPELINE_LOOKBACK_COUNT; i++) {
    previousPipelineStages.push_back(vector<bytes>(5));
  }
};

Element EmulatorScreen::renderPipeline(vector<bytes> stages) {
  Elements renderedStages;

  // for (uint i=0; i < PIPELINE_LOOKBACK_COUNT; i++) {
  //   renderedStages.push_back(
  //     hbox(
  //       text(L" T-"),
  //       text(stringToWString(to_string(PIPELINE_LOOKBACK_COUNT - i))) | size(WIDTH, EQUAL, 2),
  //       text(stringToWString(getBytesForPrint(previousPipelineStages[i][0]))) | center | size(WIDTH, EQUAL, (2 * XLEN) + XLEN) | color(Color::Grey50),
  //       text(L"   "),
  //       text(stringToWString(getBytesForPrint(previousPipelineStages[i][1]))) | center | size(WIDTH, EQUAL, (2 * XLEN) + XLEN) | color(Color::Grey50),
  //       text(L"   "),
  //       text(stringToWString(getBytesForPrint(previousPipelineStages[i][2]))) | center | size(WIDTH, EQUAL, (2 * XLEN) + XLEN) | color(Color::Grey50),
  //       text(L"   "),
  //       text(stringToWString(getBytesForPrint(previousPipelineStages[i][3]))) | center | size(WIDTH, EQUAL, (2 * XLEN) + XLEN) | color(Color::Grey50),
  //       text(L"   "),
  //       text(stringToWString(getBytesForPrint(previousPipelineStages[i][4]))) | center | size(WIDTH, EQUAL, (2 * XLEN) + XLEN) | color(Color::Grey50)
  //     )
  //   );
  // }

  renderedStages.push_back(
    hbox(
        text(L"") | size(WIDTH, EQUAL, 1),
        text(stringToWString(getBytesForPrint(stages[0]))) | center | size(WIDTH, EQUAL, (2 * XLEN) + XLEN) | color(Color::Grey58),
        text(L"   "),
        text(stringToWString(getBytesForPrint(stages[1]))) | center | size(WIDTH, EQUAL, (2 * XLEN) + XLEN) | color(Color::Grey58),
        text(L"   "),
        text(stringToWString(getBytesForPrint(stages[2]))) | center | size(WIDTH, EQUAL, (2 * XLEN) + XLEN) | color(Color::Grey58),
        text(L"   "),
        text(stringToWString(getBytesForPrint(stages[3]))) | center | size(WIDTH, EQUAL, (2 * XLEN) + XLEN) | color(Color::Grey58),
        text(L"   "),
        text(stringToWString(getBytesForPrint(stages[4]))) | center | size(WIDTH, EQUAL, (2 * XLEN) + XLEN) | color(Color::Grey58)
      )
  );

  Element box = vbox(
    text(L" Pipeline Stages") | color(Color::Grey78),
    separator(),
    hbox(
      text(L"") | size(WIDTH, EQUAL, 1),
      text(L"IF") | center | size(WIDTH, EQUAL, (2 * XLEN) + XLEN),
      text(L"   "),
      text(L"ID") | center | size(WIDTH, EQUAL, (2 * XLEN) + XLEN),
      text(L"   "),
      text(L"EX") | center | size(WIDTH, EQUAL, (2 * XLEN) + XLEN),
      text(L"   "),
      text(L"MEM") | center | size(WIDTH, EQUAL, (2 * XLEN) + XLEN),
      text(L"   "),
      text(L"WB") | center | size(WIDTH, EQUAL, (2 * XLEN) + XLEN)
    ) | color(Color::Grey78),
    move(renderedStages)
  ) | border | color(Color::Grey93);

  return box;
};

Element EmulatorScreen::renderRegisterFile(vector<bytes> registerValues) {
  Elements rows;

  for (uint i=0; i < registerValues.size(); i+=2) {
    rows.push_back(
      vbox(
        hbox(
          hbox(
            text(L"x"),
            text(stringToWString(to_string(i))),
            text(L" ")
          ) | align_right | size(WIDTH, EQUAL, 4) | color(Color::Grey82),
          text(stringToWString(getBytesForPrint(registerValues[i]))) | color(Color::Grey58),
          text(L"   "),
          hbox(
            text(L"x"),
            text(stringToWString(to_string(i))),
            text(L" ")
          ) | align_right | size(WIDTH, EQUAL, 4) | color(Color::Grey82),
          text(stringToWString(getBytesForPrint(registerValues[i+1]))) | color(Color::Grey58)
        )
      )
    );
  }

  return vbox(
    text(L" Registers") | color(Color::Grey82),
    separator(),
    move(rows)
  ) | border | color(Color::Grey93);
}

void EmulatorScreen::render(vector<bytes> pipelineStage, vector<bytes> registerValues, vector<bytes> memorySegment, string stdoutMessage) {
  Element output = vbox(
    renderPipeline(pipelineStage)
  );

  Screen screen = Screen::Create(Dimension::Fit(output), Dimension::Fit(output));
  Render(screen, output.get());

  cout << resetPosition << screen.ToString() << flush;
  resetPosition = screen.ResetPosition();
}
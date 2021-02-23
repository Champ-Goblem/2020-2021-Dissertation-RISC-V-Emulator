#include "../include/screen/screen.h"
#include "../include/bytemanip.h"
#include "../include/screen/helpers.h"
#include "../lib/headers/ftxui/component/input.hpp"

EmulatorScreen::EmulatorScreen(ushort XLEN, vector<ButtonMetadata> buttonMetadata): buttonComponent(buttonMetadata) {
  this->screeni = new ScreenInteractive(0, 0, ScreenInteractive::Dimension::TerminalOutput, false);
  this->XLEN = XLEN;
  this->width = (XLEN * 3 * 5) + (3 * 5) + 3;
  this->registerWidth = 13 + (XLEN * 2 * 6);
  cout << registerWidth << endl;
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
    ) | color(Color::Grey78) | center,
    move(renderedStages) | center
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
          text(L" "),
          hbox(
            text(L"x"),
            text(stringToWString(to_string(i+1))),
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

Element EmulatorScreen::renderMemory(vector<byte> memorySegment, ulong addr) {
  Elements entries;
  Elements addresses;

  uint outputWidth = ((width - 20) / 4);

  for (uint i=0; i < memorySegment.size(); i+= outputWidth) {
    addresses.push_back(
      hbox(
        text(L" "),
        text(ulongToHexWstring((addr + i))) | align_right | size(WIDTH, EQUAL, (sizeof(addr) * 2)),
        text(L" ")
      )
    );

    Elements row;
    for (uint c=0; c < outputWidth; c++) {
      row.push_back(
        hbox(
          text(L" "),
          text(byteToHexWstring(memorySegment[i+c])),
          text(L" ")
        )
      );
    }
    entries.push_back(
      vbox(
        hbox(
          move(row)
        )
      )
    );
  }

  return vbox(
    text(L" Memory") | color(Color::Grey82),
    separator(),
    hbox(
      vbox(
        move(addresses)
      ) | color(Color::Grey82),
      separator(),
      vbox(
        move(entries)
      ) | color(Color::Grey58)
    )
  ) | border | color(Color::Grey93);
}

Element EmulatorScreen::renderSTDOut(string message) {
  Elements rows;
  Elements row;
  for (uint i=0; i < message.length(); i++) {
    char& c = message.at(i);
    if (c == '\n') {
      rows.push_back(
        vbox(
          hbox(
            text(L" "),
            move(row)
          )
        )
      );
      row = Elements(0);
    } else if (c == '\t') {
      row.push_back(
        text(L"   ")
      );
    } else {
      string str(1, c);
      row.push_back(
        text(stringToWString(str))
      );
    }
  }

  if (row.size() > 0) {
    rows.push_back(
      vbox(
        hbox(
          text(L" "),
          move(row)
        )
      )
    );
  }

  return vbox(
    move(rows)
  ) | border;
}

void EmulatorScreen::render(vector<bytes> pipelineStage, vector<bytes> registerValues, vector<byte> memorySegment, string stdoutMessage, ulong startAddr) {
  if (buttonController.joinable()) {
    screeni->ExitLoopClosure()();
    buttonController.join();
    resetPosition += screeni->ResetPosition();
    resetPosition += screeni->ResetPosition();
    resetPosition += screeni->ResetPosition();
    screeni = new ScreenInteractive(0, 0, ScreenInteractive::Dimension::TerminalOutput, false);
  }

  Element output = vbox(
    hbox(
      move(renderPipeline(pipelineStage)) | size(WIDTH, EQUAL, width)
    ),
    hbox(
      move(renderMemory(memorySegment, startAddr)) | size(WIDTH, EQUAL, width)
    ),
    hbox(
      hbox(
        move(renderRegisterFile(registerValues))
      ) | size (WIDTH, EQUAL, registerWidth),
      hbox(
        move(renderSTDOut(stdoutMessage)) | size(WIDTH, EQUAL, width)
      ) 
    ) | size(WIDTH, EQUAL, width - 2)
  );

  Screen screen = Screen::Create(Dimension::Full(), Dimension::Fit(output));
  Render(screen, output.get());

  cout << resetPosition << screen.ToString() << endl << flush;
  resetPosition = screen.ResetPosition();
  buttonController = thread(&ScreenInteractive::Loop, screeni, &buttonComponent);
}

EmulatorScreen::~EmulatorScreen() {
  if (buttonController.joinable()) {
    screeni->ExitLoopClosure()();
    buttonController.join();
  }
  delete(screeni);
}

ButtonComponent::ButtonComponent(vector<ButtonMetadata> buttonMetadata) {
  Add(&container);

  for (uint i=0; i < buttonMetadata.size(); i++) {
    Button* b = new Button(stringToWString(buttonMetadata[i].text));
    b->on_click = buttonMetadata[i].fn;
    if (i == 0) b->Active();
    container.Add(b);
    buttons.push_back(move(b));
  }
}

ButtonComponent::~ButtonComponent() {
  for (uint i=0; i < buttons.size(); i++) {
    delete(buttons[i]);
  }
  buttons.~vector();
}
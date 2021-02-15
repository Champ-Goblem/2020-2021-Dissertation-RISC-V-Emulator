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

Element EmulatorScreen::renderMemory(vector<bytes> memorySegment, ulong addr) {
  Elements entries;
  Elements addresses;

  for (uint i=0; i < memorySegment.size(); i+= MEMORY_OUTPUT_WIDTH) {
    addresses.push_back(
      hbox(
        text(L" "),
        text(stringToWString(to_string((addr + i)))) | align_right | size(WIDTH, EQUAL, 8),
        text(L" ")
      )
    );

    Elements row;
    for (uint c=0; c < MEMORY_OUTPUT_WIDTH; c++) {
      row.push_back(
        hbox(
          text(L" "),
          text(stringToWString(getBytesForPrint(memorySegment[i+c]))),
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
          move(row)
        )
      )
    );
  }
  return frame(
    vbox(
      move(rows)
    )
  ) | size(HEIGHT, EQUAL, 5) | border;
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
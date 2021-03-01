#ifndef __Main__
#define __Main__

#include "emustd.h"
#include "screen/screen.h"

string printArgs();
string stringSplit(string arg, char delim);
Config parseArgs(int argc, char** argv);
void start();
void stop();
void pause();
void resume();
void step();
void renderUI(EmulatorScreen* screen, Processor* processor, string output="", bool stopRenderThread=true);
void parseInputContent(string content);

#endif
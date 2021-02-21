#ifndef __Main__
#define __Main__

#include "emustd.h"
#include "screen/screen.h"

string printArgs();
string getArgParameter(string arg);
Config parseArgs(int argc, char** argv);
void start(Processor* processor);
void stop();
void pause(Processor* processor);
void resume(Processor* processor);
void step(Processor* processor);
void renderUI(EmulatorScreen* screen, Processor* processor, string output="");

#endif
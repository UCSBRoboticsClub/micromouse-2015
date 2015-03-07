#include "Commands.h"
#include "Globals.h"
#include "RadioTerminal.h"
#include <functional>
#include <cstring>
#include <stdio.h>


struct Parameter
{
    const char* name;
    float* var;
};

struct Variable
{
    const char* name;
    std::function<float(void)> func;
};


Parameter paramList[] =
{
};


Variable varList[] =
{
    {"rv", [&]{ return rightWheel.getVelocity(); } },
    {"lv", [&]{ return leftWheel.getVelocity(); } },
    {"rc", [&]{ return rightWheel.velocityControl; } },
    {"lc", [&]{ return leftWheel.velocityControl; } },
    {"rd", [&]{ return rightSensor.getDistance(); } },
    {"ld", [&]{ return leftSensor.getDistance(); } },
    {"fd", [&]{ return frontSensor.getDistance(); } },
};


class WatchHandler : public CmdHandler
{
public:
    WatchHandler(std::function<float(void)> wf);
    virtual void sendChar(char c) { timer.end(); RadioTerminal::terminateCmd(); }
    static std::function<float(void)> watchFun;
    static IntervalTimer timer;
    static void refresh();
};

std::function<float(void)> WatchHandler::watchFun;
IntervalTimer WatchHandler::timer;


CmdHandler* watch(const char* input)
{
    const char* s = std::strchr(input, ' ');
    const int varListSize = (sizeof varList) / (sizeof varList[0]);

    if (s != NULL)
    {
        ++s;
        for (int i = 0; i < varListSize; ++i)
        {
            if (std::strncmp(s, varList[i].name, 32) == 0)
                return new WatchHandler(varList[i].func);
        }
    }

    RadioTerminal::write(s);
    RadioTerminal::write("Usage: w <var>\nValid vars:");
    char buf[16];
    for (int i = 0; i < varListSize; ++i)
    {
        snprintf(buf, 32, "\n  %s", varList[i].name);
        RadioTerminal::write(buf);
    }
    return NULL;
}


WatchHandler::WatchHandler(std::function<float(void)> wf)
{
    watchFun = wf;
    timer.begin(&WatchHandler::refresh, 200000);
}


void WatchHandler::refresh()
{
    char output[256];

    sprintf(output, "\r         \r\r\r%4.4f", watchFun());
    RadioTerminal::write(output);
}


void setupCommands()
{
    RadioTerminal::addCommand("w", &watch);
}


// Workaround for teensyduino bug
void std::__throw_bad_function_call() {};

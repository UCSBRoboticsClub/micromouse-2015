#include "Commands.h"
#include "Globals.h"
#include "RadioTerminal.h"
#include <functional>


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
    
    {
        RadioTerminal::write("error reading input parameters");
        return NULL;
    }
}


WatchHandler::WatchHandler(std::function<float(void)> wf)
{
    watchFun = wf;
    timer.begin(&WatchHandler::refresh, 200000);
}


void WatchHandler::refresh()
{
    char output[256];

    sprintf(output, "\r         \r\r\r%4.4f", *watch);
    RadioTerminal::write(output);
}


void setupCommands()
{
    RadioTerminal::addCommand("w", &watch);
}

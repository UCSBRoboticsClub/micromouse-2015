#include "Commands.h"
#include "Globals.h"
#include "RadioTerminal.h"
#include <IntervalTimer.h>
#undef min
#undef max
#include <functional>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>

#define GET(name) [&]{ return float(name); }
#define SETFLOAT(name) [&](float f){ name = f; }


struct Setter
{
    const char* name;
    std::function<void(float)> func;
};

struct Getter
{
    const char* name;
    std::function<float(void)> func;
};


const Setter setList[] =
{
    {"rw.vkp", SETFLOAT(rightWheel.velocityLoop.kp)},
    {"rw.vki", SETFLOAT(rightWheel.velocityLoop.ki)},
    {"rw.vkd", SETFLOAT(rightWheel.velocityLoop.kd)},
    {"lw.vkp", SETFLOAT(leftWheel.velocityLoop.kp)},
    {"lw.vki", SETFLOAT(leftWheel.velocityLoop.ki)},
    {"lw.vkd", SETFLOAT(leftWheel.velocityLoop.kd)},
    {"rw.vs", [&](float f){ rightWheel.setVelocity(f); }},
    {"lw.vs", [&](float f){ leftWheel.setVelocity(f); }},
    {"rw.pkp", SETFLOAT(rightWheel.positionLoop.kp)},
    {"rw.pki", SETFLOAT(rightWheel.positionLoop.ki)},
    {"rw.pkd", SETFLOAT(rightWheel.positionLoop.kd)},
    {"lw.pkp", SETFLOAT(leftWheel.positionLoop.kp)},
    {"lw.pki", SETFLOAT(leftWheel.positionLoop.ki)},
    {"lw.pkd", SETFLOAT(leftWheel.positionLoop.kd)},
    {"rw.ps", [&](float f){ rightWheel.setPosition(f); }},
    {"lw.ps", [&](float f){ leftWheel.setPosition(f); }},
};


const Getter getList[] =
{
    {"x", GET(x)},
    {"y", GET(y)},
    {"theta", GET(theta)},
    {"rw.vkp", GET(rightWheel.velocityLoop.kp)},
    {"rw.vki", GET(rightWheel.velocityLoop.ki)},
    {"rw.vkd", GET(rightWheel.velocityLoop.kd)},
    {"lw.vkp", GET(leftWheel.velocityLoop.kp)},
    {"lw.vki", GET(leftWheel.velocityLoop.ki)},
    {"lw.vkd", GET(leftWheel.velocityLoop.kd)},
    {"rw.vs", GET(rightWheel.velocitySetpoint)},
    {"lw.vs", GET(leftWheel.velocitySetpoint)},
    {"rw.v", GET(rightWheel.getVelocity())},
    {"lw.v", GET(leftWheel.getVelocity())},
    {"rw.vc", GET(rightWheel.velocityControl)},
    {"lw.vc", GET(leftWheel.velocityControl)},
    {"rw.pkp", GET(rightWheel.positionLoop.kp)},
    {"rw.pki", GET(rightWheel.positionLoop.ki)},
    {"rw.pkd", GET(rightWheel.positionLoop.kd)},
    {"lw.pkp", GET(leftWheel.positionLoop.kp)},
    {"lw.pki", GET(leftWheel.positionLoop.ki)},
    {"lw.pkd", GET(leftWheel.positionLoop.kd)},
    {"rw.ps", GET(rightWheel.positionSetpoint)},
    {"lw.ps", GET(leftWheel.positionSetpoint)},
    {"rw.p", GET(rightWheel.getPosition())},
    {"lw.p", GET(leftWheel.getPosition())},
    {"rw.enc", GET(rightWheel.getCounts())},
    {"lw.enc", GET(leftWheel.getCounts())},
    {"rs.d", GET(rightSensor.getDistance())},
    {"ls.d", GET(leftSensor.getDistance())},
    {"fs.d", GET(frontSensor.getDistance())},
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
    char buf[32];
    const int getListSize = (sizeof getList) / (sizeof getList[0]);

    const char* s = std::strchr(input, ' ');
    if (s != nullptr)
    {
        ++s;
        for (int i = 0; i < getListSize; ++i)
        {
            if (std::strncmp(s, getList[i].name, 32) == 0)
                return new WatchHandler(getList[i].func);
        }
    }

    RadioTerminal::write("Usage: w <var>\nValid vars:");
    for (int i = 0; i < getListSize; ++i)
    {
        snprintf(buf, 32, "\n  %s", getList[i].name);
        RadioTerminal::write(buf);
    }
    return nullptr;
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


CmdHandler* print(const char* input)
{
    char buf[32];
    const int getListSize = (sizeof getList) / (sizeof getList[0]);
    
    const char* s = std::strchr(input, ' ');
    if (s != nullptr)
    {
        ++s;
        for (int i = 0; i < getListSize; ++i)
        {
            if (std::strncmp(s, getList[i].name, 32) == 0)
            {
                snprintf(buf, 32, "%s = %4.4f",
                         getList[i].name,
                         getList[i].func());
                RadioTerminal::write(buf);
                return nullptr;
            }
        }
    }
    
    RadioTerminal::write("Usage: p <var>\nValid variables:");
    for (int i = 0; i < getListSize; ++i)
    {
        snprintf(buf, 32, "\n  %s", getList[i].name);
        RadioTerminal::write(buf);
    }
    return nullptr;
}


CmdHandler* set(const char* input)
{
    char buf[32];
    const int setListSize = (sizeof setList) / (sizeof setList[0]);
    
    const char* s = std::strchr(input, ' ');
    if (s != nullptr)
    {
        const char* s2 = std::strchr(++s, ' ');
        if (s2 != nullptr)
        {
            int pslen = s2 - s;
            float value = strtof(++s2, nullptr);
            
            for (int i = 0; i < setListSize; ++i)
            {
                if (std::strncmp(s, setList[i].name, pslen) == 0)
                {
                    setList[i].func(value);
                    snprintf(buf, 32, "%s = %4.4f",
                             setList[i].name,
                             value);
                    RadioTerminal::write(buf);
                    return nullptr;
                }
            }
        }
    }
    
    RadioTerminal::write("Usage: s <var> <value>\nValid variables:");
    for (int i = 0; i < setListSize; ++i)
    {
        snprintf(buf, 32, "\n  %s", setList[i].name);
        RadioTerminal::write(buf);
    }
    return nullptr;
}


void setupCommands()
{
    RadioTerminal::addCommand("w", &watch);
    RadioTerminal::addCommand("p", &print);
    RadioTerminal::addCommand("s", &set);
}


// Workaround for teensyduino/libstdc++ bug
void std::__throw_bad_function_call() { while(true) {} };

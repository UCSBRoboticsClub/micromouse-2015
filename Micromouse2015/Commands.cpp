#include "Commands.h"
#include "Globals.h"

/*
 class WatchHandler : public CmdHandler
{
public:
    WatchHandler(float* w);
    virtual void sendChar(char c) { timer.end(); RadioTerminal::terminateCmd(); }
    static float* watch;
    IntervalTimer timer;
    static void refresh();
};

float* WatchHandler::watch;


CmdHandler* watch(const char* input)
{
    // Read command parameters
    if (!strncmp(input, "w v1", 8))
    {
        return new WatchHandler(&v1);
    }
    else if (!strncmp(input, "w v2", 8))
    {
        return new WatchHandler(&v2);
    }
    else if (!strncmp(input, "w x1", 8))
    {
        return new WatchHandler(&x1);
    }
    else if (!strncmp(input, "w x2", 8))
    {
        return new WatchHandler(&x2);
    }
    else if (!strncmp(input, "w error", 8))
    {
        return new WatchHandler(&errorF);
    }
    else if (!strncmp(input, "w loadmax", 8))
    {
        return new WatchHandler(&loadMax);
    }
    else if (!strncmp(input, "w load", 8))
    {
        return new WatchHandler(&loadPercentF);
    }
    else if (!strncmp(input, "w vinmax", 8))
    {
        return new WatchHandler(&vinmax);
    }
    else
    {
        RadioTerminal::write("error reading input parameters");
        return NULL;
    }
}


WatchHandler::WatchHandler(float* w)
{
    watch = w;
    timer.begin(&WatchHandler::refresh, 200000);
}


void WatchHandler::refresh()
{
    char output[256];

    sprintf(output, "\r         \r\r\r%4.4f", *watch);
    RadioTerminal::write(output);
}


CmdHandler* setkp(const char* input)
{
    char output[256];
    
    float kp;
    sscanf(input, "kp %f", &kp);
    steerLoop.setKp(kp);
    sprintf(output, "kp = %f", kp);
    RadioTerminal::write(output);
    
    return NULL;
}


CmdHandler* setki(const char* input)
{
    char output[256];
    
    float ki;
    sscanf(input, "ki %f", &ki);
    steerLoop.setKi(ki);
    sprintf(output, "ki = %f", ki);
    RadioTerminal::write(output);
    
    return NULL;
}


CmdHandler* setkd(const char* input)
{
    char output[256];
    
    float kd;
    sscanf(input, "kd %f", &kd);
    steerLoop.setKd(kd);
    sprintf(output, "kd = %f", kd);
    RadioTerminal::write(output);
    
    return NULL;
}


CmdHandler* seth(const char* input)
{
    char output[256];
    
    sscanf(input, "h %f", &h);
    sprintf(output, "h = %f", h);
    RadioTerminal::write(output);
    
    return NULL;
}


CmdHandler* setd(const char* input)
{
    char output[256];
    
    sscanf(input, "d %f", &d);
    sprintf(output, "d = %f", d);
    RadioTerminal::write(output); 
    
    return NULL;
}


CmdHandler* setspeed(const char* input)
{
    char output[256];
    
    sscanf(input, "speed %f", &speed);
    sprintf(output, "speed = %f", speed);
    RadioTerminal::write(output); 
    
    return NULL;
}


CmdHandler* dumplogm(const char* input)
{
    char buffer[256];
    
    RadioTerminal::write("[");
    
    for (int i = dataLog.size(); i > 0; --i)
    {
        snprintf(buffer, 256, "%e,%e;\n",
                 dataLog[i].error,
                 dataLog[i].control);
        RadioTerminal::write(buffer); 
    }
    
    snprintf(buffer, 256, "%e,%e];",
             dataLog[0].error,
             dataLog[0].control);
    RadioTerminal::write(buffer); 
    
    return NULL;
}


CmdHandler* dumplog(const char* input)
{
    char buffer[256];
    
    for (int i = dataLog.size(); i >= 0; --i)
    {
        snprintf(buffer, 256, "%e,%e\n",
                 dataLog[i].error,
                 dataLog[i].control);
        RadioTerminal::write(buffer); 
    }
    
    return NULL;
}
*/

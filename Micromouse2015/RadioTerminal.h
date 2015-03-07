#ifndef RADIOTERMINAL_H
#define RADIOTERMINAL_H

#include <cstdint>

#define INPUT_BUFFER_MAX 32
#define NUM_COMMANDS_MAX 16

#define USB_SERIAL_ENABLE


class CmdHandler
{
public:
    virtual ~CmdHandler() {}
    virtual void sendChar(char c) {}
};


namespace RadioTerminal
{
    void initialize(int csnPin = -1, int cePin = -1, int irqPin = -1);
    void reset();
    void addCommand(const char* cmdString, CmdHandler* (*fpointer)(const char*) );
    void terminateCmd();
    void write(const char* string);
    
    extern uint8_t channel;
    extern uint32_t rxAddress;
    extern uint32_t txAddress;
};


void serialEvent();


#endif // RADIOTERMINAL_H

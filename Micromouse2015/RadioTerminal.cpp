#include "RadioTerminal.h"
#include <Arduino.h>
#include "nRF24L01P_defs.h"
#include <SPI.h>
#include <IntervalTimer.h>
#include <cctype>
#include <cstring>
#include <stdio.h>


namespace RadioTerminal
{
    // Internal functions and variables
    int getRegister(int address);
    int getStatus();
    void setRegister(int address, int data);
    void receive();
    void clear();
    void transmit(uint32_t data);
    void receiveChar(char c);

    int _csnPin;
    int _cePin;
    int _irqPin;
    bool useRadio;
    
    uint8_t channel = 56;
    uint32_t rxAddress = 0xd191bb;
    uint32_t txAddress = 0xe6e1fa;

    char inputBufferA[INPUT_BUFFER_MAX];
    char inputBufferB[INPUT_BUFFER_MAX];
    char* inputBuffer = inputBufferA;
    CmdHandler* runningCmd;
    uint8_t escapeMode = 0;
    int bufPos = 0;
    int bufLen = 0;
    
    struct cmd
    {
        char cmdString[INPUT_BUFFER_MAX];
        int stringLength;
        CmdHandler* (*fpointer)(const char*);
        cmd() { stringLength = 0; }
    };
    
    cmd cmdList[NUM_COMMANDS_MAX];
    int numCommands;
    
    #ifdef USB_SERIAL_ENABLE
    void serialEvent();
    #endif


    void initialize(int csnPin, int cePin, int irqPin)
    {
        _csnPin = csnPin;
        _cePin = cePin;
        _irqPin = irqPin;

        if (csnPin < 0 || cePin < 0 || irqPin < 0)
        {
            useRadio = false;
        }
        else
        {
            useRadio = true;
            
            // Set up pins
            pinMode(_csnPin, OUTPUT);
            pinMode(_cePin, OUTPUT);
            pinMode(_irqPin, OUTPUT);

            // Disable nRF24L01+
            digitalWrite(_cePin, 0);
        
            // Disable chip select
            digitalWrite(_csnPin, 1);
        
            // Set up SPI
            SPI.begin();
            SPI.setClockDivider(SPI_CLOCK_DIV8);
        
            // Set up IRQ
            pinMode(_irqPin, INPUT);
            attachInterrupt(irqPin, receive, FALLING);
        
            // These values need to be initialized
            numCommands = 0;
            runningCmd = NULL;
            inputBuffer[0] = '\0';

            reset();
        }
        
        #ifdef USB_SERIAL_ENABLE
        Serial.begin(115200);
        #endif
    }


    void reset()
    {
        // Wait for power on reset
        delayMicroseconds(TIMING_Tpor);

        // Put into standby
        digitalWrite(_cePin, 0);
        
        // Configure registers
        int config = CONFIG_MASK_TX_DS | CONFIG_MASK_MAX_RT | CONFIG_EN_CRC | CONFIG_PWR_UP | CONFIG_PRIM_RX;
        setRegister(CONFIG, config);

        // Check whether radio is present/working
        if (getRegister(CONFIG) != config)
        {
            useRadio = false;
            return;
        }

        // Configure remaining registers
        setRegister(EN_AA, 0x00);
        setRegister(EN_RXADDR, ERX_P1);
        setRegister(SETUP_AW, SETUP_AW_3BYTES);
        setRegister(SETUP_RETR, 0x00);
        setRegister(RF_CH, channel);
        setRegister(RF_SETUP, RF_SETUP_RF_DR_HIGH | RF_SETUP_RF_PWR_0);
        setRegister(STATUS, STATUS_RX_DR | STATUS_TX_DS | STATUS_MAX_RT);
        setRegister(RX_PW_P1, 4);
        setRegister(DYNPD, 0x00);
        setRegister(FEATURE, 0x00);
        
        // Set addresses
        digitalWrite(_csnPin, 0);
        SPI.transfer(W_REGISTER | RX_ADDR_P1);
        SPI.transfer((rxAddress >>  0) & 0xff);
        SPI.transfer((rxAddress >>  8) & 0xff);
        SPI.transfer((rxAddress >> 16) & 0xff);
        digitalWrite(_csnPin, 1);
        digitalWrite(_csnPin, 0);
        SPI.transfer(W_REGISTER | TX_ADDR);
        SPI.transfer((txAddress >>  0) & 0xff);
        SPI.transfer((txAddress >>  8) & 0xff);
        SPI.transfer((txAddress >> 16) & 0xff);
        digitalWrite(_csnPin, 1);
        
        // Put into PRX
        digitalWrite(_cePin, 1);
        delayMicroseconds(TIMING_Tstby2a);
        
        // Flush FIFOs
        digitalWrite(_csnPin, 0);
        SPI.transfer(FLUSH_TX);
        digitalWrite(_csnPin, 1);
        digitalWrite(_csnPin, 0);
        SPI.transfer(FLUSH_RX);
        digitalWrite(_csnPin, 1);
        
        write("\n> ");
    }
    
    
    void addCommand(const char* cmdString, CmdHandler* (*fpointer)(const char*) )
    {
        // Check if there's room in the command array before adding a command
        if (numCommands < NUM_COMMANDS_MAX)
        {
            strncpy(cmdList[numCommands].cmdString, cmdString, INPUT_BUFFER_MAX);
            
            // Make sure that the command string is null terminated
            cmdList[numCommands].cmdString[INPUT_BUFFER_MAX - 1] = '\0'; 
            cmdList[numCommands].stringLength = strlen(cmdList[numCommands].cmdString);
            cmdList[numCommands].fpointer = fpointer;
            ++numCommands;
        }
        else
        {
            write("error: too many commands\n");
        }
    }


    void transmit(uint32_t data)
    {
        if (useRadio)
        {
            // Put into standby
            digitalWrite(_cePin, 0);
        
            // Configure for PTX
            int config = getRegister(CONFIG);
            config &= ~CONFIG_PRIM_RX;
            setRegister(CONFIG, config);
        
            // Write packet data
            digitalWrite(_csnPin, 0);
            SPI.transfer(W_TX_PAYLOAD);
            SPI.transfer( (data>>0) & 0xff );
            SPI.transfer( (data>>8) & 0xff );
            SPI.transfer( (data>>16) & 0xff );
            SPI.transfer( (data>>24) & 0xff );
            digitalWrite(_csnPin, 1);
        
            // Put into PTX
            digitalWrite(_cePin, 1);
            delayMicroseconds(TIMING_Tstby2a);
            digitalWrite(_cePin, 0);
        
            // Wait for message transmission and put into PRX
            delayMicroseconds(TIMING_Toa);
            config = getRegister(CONFIG);
            config |= CONFIG_PRIM_RX;
            setRegister(CONFIG, config);
            setRegister(STATUS, STATUS_TX_DS);
            digitalWrite(_cePin, 1);
        }
        
        #ifdef USB_SERIAL_ENABLE
        Serial.write( (data>>0) & 0xff );
        Serial.write( (data>>8) & 0xff );
        Serial.write( (data>>16) & 0xff );
        Serial.write( (data>>24) & 0xff );
        #endif
    }


    int getRegister(int address)
    {
        digitalWrite(_csnPin, 0);
        int rc = R_REGISTER | (address & REGISTER_ADDRESS_MASK);
        SPI.transfer(rc);
        int data = SPI.transfer(NOP);
        digitalWrite(_csnPin, 1);
        return data;
    }


    int getStatus()
    {
        digitalWrite(_csnPin, 0);
        int status = SPI.transfer(NOP);
        digitalWrite(_csnPin, 1);
        return status;
    }


    void setRegister(int address, int data)
    {
        bool enabled = false;
        if (digitalRead(_cePin) == 1)
        {
            enabled = true;
            digitalWrite(_cePin, 0);
        }
        
        digitalWrite(_csnPin, 0);
        int rc = W_REGISTER | (address & REGISTER_ADDRESS_MASK);
        SPI.transfer(rc);
        SPI.transfer(data & 0xff);
        digitalWrite(_csnPin, 1);
        
        if (enabled)
        {
            digitalWrite(_cePin, 1);
            delayMicroseconds(TIMING_Tpece2csn);
        }
    }


    void receive()
    {
        uint32_t data = 0;
        int pipe;
        
        while (!(getRegister(FIFO_STATUS) & FIFO_STATUS_RX_EMPTY))
        {
            // Check data pipe
            pipe = getStatus() & STATUS_RN_P_MASK;
            
            // Read data
            digitalWrite(_csnPin, 0);
            SPI.transfer(R_RX_PAYLOAD);
            data |= SPI.transfer(NOP)<<0;
            data |= SPI.transfer(NOP)<<8;
            data |= SPI.transfer(NOP)<<16;
            data |= SPI.transfer(NOP)<<24;
            digitalWrite(_csnPin, 1);
            
            // Sort into receive buffer
            switch(pipe)
            {
            case STATUS_RN_P_NO_P1:
                /* Break data message into four chars and send to terminal,
                   stop if a null terminator is found */
                for (int i = 0; i < 4; ++i)
                {
                    if ( ((data>>(8*i)) & 0xff) == '\0')
                        break;
                    receiveChar( (char)((data>>(8*i)) & 0xff) );
                }
                break;
                
            default:
                break;
            }
        }
        
        // Reset IRQ pin
        setRegister(STATUS, STATUS_RX_DR);
    }
    
    
    void receiveChar(char c)
    {
        // Check if a command is currently running
        if (runningCmd == NULL) // No command is currently running
        {
            if (escapeMode == 1) // Check for second character of escape code
            {
                if (c == '[')
                    escapeMode = 2;
                else
                    escapeMode = 0;
            }
            else if (escapeMode > 1) // Handle escape codes
            {
                switch (c)
                {
                case 'A':
                case 'B':
                    inputBuffer = (inputBuffer == inputBufferA) ? inputBufferB : inputBufferA;
                    write("\r                                        \r> ");
                    write(inputBuffer);
                    bufLen = strlen(inputBuffer);
                    bufPos = bufLen;
                    break;
                case 'C':
                    if (bufPos < bufLen)
                    {
                        transmit(inputBuffer[bufPos]);
                        ++bufPos;
                    }
                    break;
                case 'D':
                    if (bufPos > 0)
                    {
                        transmit('\b');
                        --bufPos;
                    }
                    break;
                }
                
                escapeMode = 0;
            }
            else if (isprint(c)) // If c is a printable character
            {
                // Add the new character to the input buffer and display it
                if (bufLen < INPUT_BUFFER_MAX - 1)
                {
                    if (bufPos >= bufLen)
                    {
                    
                        inputBuffer[bufLen] = c;
                        inputBuffer[++bufLen] = '\0';
                        bufPos = bufLen;
                        transmit(c);
                    }
                    else
                    {
                        ++bufLen;
                        for (int i = bufLen; i > bufPos; --i)
                            inputBuffer[i] = inputBuffer[i-1];
                        inputBuffer[bufPos++] = c;
                        write(inputBuffer + bufPos - 1);

                        int numchars = bufLen - bufPos;
                        for (; numchars / 4 > 0; numchars -= 4)
                            write("\b\b\b\b");
                        for (; numchars > 0; --numchars)
                            transmit('\b');
                    }
                }
            }
            else if (c == '\b' || c == 127) // Backspace or DEL
            {
                if (bufPos > 0)
                {
                    if (bufPos == bufLen)
                    {
                        inputBuffer[--bufPos] = '\0';
                        --bufLen;
                        write("\b \b");
                    }
                    else
                    {
                        --bufPos;
                        --bufLen;
                        transmit('\b');
                        for (int i = bufPos; i <= bufLen; ++i)
                            inputBuffer[i] = inputBuffer[i+1];
                        
                        write(inputBuffer + bufPos);
                        write("  \b\b");
                        
                        int numchars = bufLen - bufPos;
                        for (; numchars / 4 > 0; numchars -= 4)
                            write("\b\b\b\b");
                        for (; numchars > 0; --numchars)
                            transmit('\b');
                    }
                }
            }
            else if (c == '\n' || c == '\r') // Execute input command
            {
                write("\n");
                
                bool matchFound = false; // Initialize flag
                
                // Try to match the input string to a command
                for (int i = 0; i < numCommands; ++i)
                {
                    if (cmdList[i].stringLength &&
                        !strncmp(inputBuffer,
                                 cmdList[i].cmdString,
                                 cmdList[i].stringLength))
                    {
                        // Match found, call the associated command
                        runningCmd = cmdList[i].fpointer(inputBuffer);
                        matchFound = true;
                        break;
                    }
                }
                
                if (matchFound)
                {
                    /* If the command finishes immediately, it should return
                       null instead of a pointer to a CmdHandler */
                    if (runningCmd == NULL)
                    {
                        /* Terminate the command here, because there is no
                           CmdHandler to terminate it later */
                        terminateCmd();
                    }
                }
                else
                {
                    // No match was found
                    write("Valid commands:");
                    char buf[32];
                    for (int i = 0; i < numCommands; ++i)
                    {
                        snprintf(buf, 32, "\n  %s", cmdList[i].cmdString);
                        write(buf);
                    }
                    terminateCmd();
                }
            }
            else if (c == 27)
            {
                escapeMode = 1;
            }
            // Control characters other than \n, \b, 27 (ESC), and 127 (DEL) are ignored
        }
        else // A command is running, so pass the character to the CmdHandler
        {
            runningCmd->sendChar(c);
        }
    }
    
    
    void terminateCmd()
    {
        // Delete the running command's CmdHandler, if it exists
        if (runningCmd != NULL)
        {
            delete runningCmd;
            runningCmd = NULL;
        }
        
        write("\n> ");
        inputBuffer = (inputBuffer == inputBufferA) ? inputBufferB : inputBufferA;
        bufPos = 0;
        bufLen = 0;
        inputBuffer[0] = '\0'; // Clear the input buffer
    }


    void write(const char* string)
    {
        const int maxsize = 256;
        int i = 0;
        bool crFlag = false;
        
        // Send string across radio link, grouping characters into batches
        while (i < maxsize)
        {
            uint32_t data = 0;
            int j = 0;
            
            while (j < 4)
            {
                if (string[i + j] == '\0' || i + j >= maxsize)
                    break;
                if (string[i + j] == '\n')
                {
                    crFlag = !crFlag;
                    if (crFlag)
                    {
                        data |= uint32_t('\r') << 8 * j;
                        --i;
                    }
                    else
                    {
                        data |= uint32_t('\n') << 8 * j;
                    }
                }
                else
                {
                    data |= uint32_t(string[i + j]) << 8 * j;
                }
                ++j;
            }
            
            i += j;
            transmit(data);
            if (string[i] == '\0')
                break;
            delayMicroseconds(20);
        }
    }
}

    
#ifdef USB_SERIAL_ENABLE
void serialEvent()
{
    while (Serial.available())
    {
        int c = Serial.read();
        if (c != -1)
            RadioTerminal::receiveChar(c);
    }
}
#endif

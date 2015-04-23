#include <Encoder.h>
#include <IntervalTimer.h>
#include <SPI.h>
#include "Globals.h"
#include "Songs.h"
#include "RadioTerminal.h"
#include "Commands.h"
#include "Maze.h"
#include "BFS.h"
#include <cmath>


Maze<16, 16> maze;

IntervalTimer controlTimer;
IntervalTimer sensorTimer;

void controlLoop();
void sensorLoop();


void setup()
{
    asm(".global _printf_float");
    
    pinMode(buzzerPin, OUTPUT);
    pinMode(led1Pin, OUTPUT);
    pinMode(led2Pin, OUTPUT);
    pinMode(nfaultPin, INPUT_PULLUP);
    
    leftWheel.velocityLoop.setTuning(10.f, 20.f, 0.02f);
    rightWheel.velocityLoop.setTuning(10.f, 20.f, 0.02f);
    leftWheel.velocityLoop.setDerivLowpassFreq(30.f);
    rightWheel.velocityLoop.setDerivLowpassFreq(30.f);

    controlTimer.begin(controlLoop, controlPeriodUs);
    controlTimer.priority(144);

    VL6180X::setup();
    rightSensor.init(0x31);
    frontSensor.init(0x32);
    leftSensor.init(0x33);
    
    sensorTimer.begin(sensorLoop, sensorPeriodUs);
    sensorTimer.priority(160);

    RadioTerminal::initialize();
    setupCommands();

    //playSong(mortalkombat);
}


void loop()
{
    
}


void controlLoop()
{
    rled(1);

    if (digitalRead(nfaultPin) == LOW)
        tone(buzzerPin, 440, 300);
    
    leftWheel.update();
    rightWheel.update();

    // Update state based on encoders
    const int rCountNew = rightWheel.getCounts();
    const int lCountNew = leftWheel.getCounts();
    const int rDiff = rCount - rCountNew;
    const int lDiff = lCount - lCountNew;
    rCount = rCountNew;
    lCount = lCountNew;

    const float dTheta = (rDiff - lDiff) * count2dist / wheelBase;
    const float dDist = (rDiff + lDiff) * 0.5f * count2dist;

    x += dDist * std::cos(theta + dTheta * 0.5f);
    y += dDist * std::sin(theta + dTheta * 0.5f);
    theta += dTheta;
    
    rled(0);
}


void sensorLoop()
{
    rightSensor.poll();
    frontSensor.poll();
    leftSensor.poll();
}




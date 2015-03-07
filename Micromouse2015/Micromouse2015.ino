#include <Encoder.h>
#include <IntervalTimer.h>
#include <SPI.h>
#include "Globals.h"
#include "Songs.h"
#include "RadioTerminal.h"
#include "Commands.h"


IntervalTimer controlTimer;
IntervalTimer sensorTimer;

void controlLoop();
void sensorLoop();


void setup()
{
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

    RadioTerminal::initialize(10, 9, 8);
    setupCommands();

    delay(1000);
    
    //playSong(recorder);
}


void loop()
{
    if (digitalRead(nfaultPin) == LOW)
        tone(buzzerPin, 440, 300);
    
    delay(100);
}


void controlLoop()
{
    leftWheel.update();
    rightWheel.update();
}


void sensorLoop()
{
    rightSensor.poll();
    frontSensor.poll();
    leftSensor.poll();
}




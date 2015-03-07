#include <Encoder.h>
#include <IntervalTimer.h>
#include <stdio.h>
#include "Globals.h"
#include "Songs.h"


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
    
    Serial.begin(115200);
    
    leftWheel.velocityLoop.setTuning(10.f, 20.f, 0.02f);
    rightWheel.velocityLoop.setTuning(10.f, 20.f, 0.02f);
    leftWheel.velocityLoop.setDerivCutoffFreq(30.f);
    rightWheel.velocityLoop.setDerivCutoffFreq(30.f);

    controlTimer.begin(controlLoop, controlPeriodUs);
    controlTimer.priority(144);

    VL6180X::setup();
    rightSensor.init(0x31);
    frontSensor.init(0x32);
    leftSensor.init(0x33);
    
    sensorTimer.begin(sensorLoop, sensorPeriodUs);
    sensorTimer.priority(160);

    delay(1000);
    
    playSong(recorder);
}


void loop()
{
    char buf[256];
    snprintf(buf, 256, "%.4f, %.4f, %.4f\n", rightSensor.getDistance(), frontSensor.getDistance(), leftSensor.getDistance());
    Serial.write(buf);

    if (frontSensor.getDistance() < 0.15)
    {
        rightWheel.setVelocity(-0.25f);
        leftWheel.setVelocity(-0.5f);
        digitalWrite(led1Pin, LOW);
        digitalWrite(led2Pin, LOW);
        tone(buzzerPin, 440, 300);
    }
    else if (leftSensor.getDistance() < 0.15)
    {
        rightWheel.setVelocity(0.f);
        leftWheel.setVelocity(0.5f);
        digitalWrite(led1Pin, LOW);
        digitalWrite(led2Pin, HIGH);
    }
    else if (rightSensor.getDistance() < 0.15)
    {
        rightWheel.setVelocity(0.5f);
        leftWheel.setVelocity(0.f);
        digitalWrite(led1Pin, HIGH);
        digitalWrite(led2Pin, LOW);
    }
    else
    {
        rightWheel.setVelocity(0.5f);
        leftWheel.setVelocity(0.5f);
        digitalWrite(led1Pin, HIGH);
        digitalWrite(led2Pin, HIGH);
    }

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




#include <Encoder.h>
#include <IntervalTimer.h>
#include "Wheel.h"
#include "RingBuffer.h"
#include "Songs.h"
#include <cstring>
#include <cstdlib>

const int motorRF = 20; // right forward motor pin
const int motorRR = 21; // right reverse
const int motorLR = 22; // left forward
const int motorLF = 23; // left reverse
const int encoderR1 = 16; // right encoder
const int encoderR2 = 17; 
const int encoderL1 = 14; // left encoder
const int encoderL2 = 15;
const int buzzerPin = 5;
const int led1Pin = 3;
const int led2Pin = 4;

const unsigned int loopFreq = 100; // Hz
const unsigned int loopPeriodUs = 1000000 / loopFreq;
const float dt = 1.f / loopFreq;
IntervalTimer loopTimer;

// 3 and 4 are motor pins, 0 and 1 are encoder pins
const float wheelCirc = 0.1f;
const int ppr = 1000;
Wheel leftWheel(motorLR, motorLF, encoderL1, encoderL2, dt, wheelCirc, ppr);
Wheel rightWheel(motorRR, motorRF, encoderR1, encoderR2, dt, wheelCirc, ppr);


void setup()
{
    pinMode(buzzerPin, OUTPUT);
    pinMode(led1Pin, OUTPUT);
    pinMode(led2Pin, OUTPUT);
    
    Serial.begin(115200);
    leftWheel.velocityLoop.setTuning(0.1f, 0.f, 0.1f);
    rightWheel.velocityLoop.setTuning(0.1f, 0.f, 0.1f);

    loopTimer.begin(controlLoop, loopPeriodUs);
    loopTimer.priority(192);

    rightWheel.setVelocity(1.f);
    leftWheel.setVelocity(1.f);

    delay(1000);
    
    playSong(recorder);
}


void loop()
{
    delay(1000);
}


void controlLoop()
{
    leftWheel.update();
    rightWheel.update();
}


void playSong(const char *s)
{    
    while (s != NULL)
    {
        s = std::strchr(s, 'f');
        if (s == NULL) break;
        int f = std::strtol(++s, const_cast<char**>(&s), 10);
        if (s == NULL)  break;
        s = std::strchr(s, 't');
        if (s == NULL)  break;
        int t = std::strtol(++s, const_cast<char**>(&s), 10);

        if (f > 0)
            tone(buzzerPin, f);
        else
            noTone(buzzerPin);
        
        delay(t);
    }
    
    noTone(buzzerPin);
}

#include <Encoder.h>
#include <IntervalTimer.h>
#include "Wheel.h"
#include "RingBuffer.h"
#include "Songs.h"
#include <cstring>
#include <cstdlib>

const int motorRF = 21; // right forward motor pin
const int motorRR = 20; // right reverse
const int motorLR = 22; // left forward
const int motorLF = 23; // left reverse
const int encoderR1 = 17; // right encoder
const int encoderR2 = 16; 
const int encoderL1 = 14; // left encoder
const int encoderL2 = 15;
const int buzzerPin = 5;
const int led1Pin = 3;
const int led2Pin = 4;

const unsigned int loopFreq = 1000; // Hz
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
    leftWheel.velocityLoop.setTuning(10.f, 20.f, 0.02f);
    rightWheel.velocityLoop.setTuning(10.f, 20.f, 0.02f);
    leftWheel.velocityLoop.setDerivCutoffFreq(30.f);
    rightWheel.velocityLoop.setDerivCutoffFreq(30.f);

    loopTimer.begin(controlLoop, loopPeriodUs);
    loopTimer.priority(192);

    delay(1000);
    
    playSong(recorder);

    rightWheel.setVelocity(0.3f);
    leftWheel.setVelocity(0.3f);
}


void loop()
{
    char buf[256];
    snprintf(buf, 256, "%.4f, %.4f\n", rightWheel.getVelocity(), leftWheel.getVelocity());
    Serial.write(buf);
    delay(100);
}


void controlLoop()
{
    digitalWrite(led1Pin, HIGH);
    leftWheel.update();
    rightWheel.update();
    digitalWrite(led1Pin, LOW);
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

#ifndef GLOBALS_H
#define GLOBALS_H

#include "Wheel.h"
#include "vl6180x.h"


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
const int nfaultPin = 7;

extern float x;
extern float y;
extern float theta;
extern int lCount;
extern int rCount;

const unsigned int controlFreq = 200; // Hz
const unsigned int controlPeriodUs = 1000000 / controlFreq;
const float dt = 1.f / controlFreq;

const float wheelCirc = 0.1f;
const int ppr = 1200;
const float count2dist = wheelCirc / ppr;
const float wheelBase = 0.823f;

const unsigned int sensorFreq = 50; // Hz
const unsigned int sensorPeriodUs = 1000000 / sensorFreq;

extern Wheel leftWheel;
extern Wheel rightWheel;

extern VL6180X rightSensor;
extern VL6180X frontSensor;
extern VL6180X leftSensor;

void rled(bool on);
void lled(bool on);


#endif // GLOBALS_H

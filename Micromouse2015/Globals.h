#ifndef GLOBALS_H
#define GLOBALS_H

#include "Wheel.h"
#include "vl6180x.h"
#include "LowPass.h"
#include "PIDController.h"
#include "BFS.h"


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

const unsigned int controlFreq = 200; // Hz
const unsigned int controlPeriodUs = 1000000 / controlFreq;
const float dt = 1.f / controlFreq;

const float wheelCirc = 0.1f;
const int ppr = 1200;
const float count2dist = wheelCirc / ppr;
const float wheelBase = 0.0823f;
const float pi = 3.14159265f;
const float cellw = 0.18f;
const float wallw = 0.012f;
const float sensw = 0.067f;
const float fsensoff = 0.054f;

const int mazem = 16;
const int mazen = 16;

const unsigned int sensorFreq = 50; // Hz
const unsigned int sensorPeriodUs = 1000000 / sensorFreq;
const float dtsensor = 1.f / sensorFreq;

struct State
{
    float x;
    float y;
    float theta;
};

extern State state;
extern State target;
extern Node currentCell;
extern int lCount;
extern int rCount;
extern LowPass drdt;
extern LowPass dfdt;
extern LowPass dldt;
extern LowPass dthdt;
extern LowPass dsdt;
extern float thgoal;
extern float therr;
extern float thctrl;
extern float speed;
extern PIDController thetaController;
extern float ctheta;
extern float cside;
extern float cfront;

extern Wheel leftWheel;
extern Wheel rightWheel;

extern VL6180X rightSensor;
extern VL6180X frontSensor;
extern VL6180X leftSensor;

void rled(bool on);
void lled(bool on);


#endif // GLOBALS_H

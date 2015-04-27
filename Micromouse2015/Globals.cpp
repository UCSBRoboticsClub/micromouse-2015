#include "Globals.h"
#include <Arduino.h>


State state = {0.f, 0.f, 0.5f*pi};
State target = {0.f, 0.f, 0.f};
Node currentCell = {0, 0};
int lCount = 0;
int rCount = 0;
LowPass drdt;
LowPass dfdt;
LowPass dldt;
LowPass dthdt;
LowPass dsdt;
float thgoal = 0.f;
float therr = 0.f;
float thctrl = 0.f;
float speed = 0.f;
PIDController thetaController(dt);

Wheel leftWheel(motorLR, motorLF, encoderL1, encoderL2, dt, wheelCirc, ppr);
Wheel rightWheel(motorRR, motorRF, encoderR1, encoderR2, dt, wheelCirc, ppr);

VL6180X rightSensor(0);
VL6180X frontSensor(1);
VL6180X leftSensor(2);


void rled(bool on)
{
    digitalWriteFast(led2Pin, on); 
}


void lled(bool on)
{
    digitalWriteFast(led1Pin, on);
}

#include "Globals.h"
#include <Arduino.h>


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

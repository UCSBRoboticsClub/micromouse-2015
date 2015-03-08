#ifndef CONTROLLOOP_H
#define CONTROLLOOP_H

#include "LowPass.h"


class ControlLoop
{
public:
    ControlLoop(float dt);
    float update(float error, float feedForward = 0.f);
    void setTuning(float kp, float ki, float kd);
    void setDerivLowpassFreq(float freq);
    void setOutputLimits(float min, float max);
    void zeroIntegral();
    
    float kp;
    float ki;
    float kd;
    
private:
    const float dt;
    float ierror;
    LowPass derror;
    LowPass ierrorlp;
    float outputMin;
    float outputMax;
    float lastError;
};

#endif // CONTROLLOOP_H

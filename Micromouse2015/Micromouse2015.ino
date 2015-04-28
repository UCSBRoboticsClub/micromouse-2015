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
float circleDist(float a, float b);
float limit(float x, float lim);


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

    drdt.setTimeConst(dtsensor, 0.1f);
    dfdt.setTimeConst(dtsensor, 0.1f);
    dldt.setTimeConst(dtsensor, 0.1f);
    dthdt.setTimeConst(dt, 0.1f);
    dsdt.setTimeConst(dt, 0.1f);

    thetaController.setTuning(1.f, 0.f, 0.01f);
    thetaController.setDerivLowpassFreq(30.f);
    thetaController.setOutputLimits(-0.2f, 0.2f);

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
    writeLog();
}


void controlLoop()
{
    rled(1);

    if (digitalRead(nfaultPin) == LOW)
        tone(buzzerPin, 440, 300);

    // Update state based on encoders
    const int rCountNew = rightWheel.getCounts();
    const int lCountNew = leftWheel.getCounts();
    const int rDiff = rCount - rCountNew;
    const int lDiff = lCount - lCountNew;
    rCount = rCountNew;
    lCount = lCountNew;

    const float dTheta = -(rDiff - lDiff) * count2dist / wheelBase;
    const float dDist = -(rDiff + lDiff) * 0.5f * count2dist;

    state.x += dDist * std::cos(state.theta + dTheta * 0.5f);
    state.y += dDist * std::sin(state.theta + dTheta * 0.5f);
    state.theta += dTheta;
    state.theta = std::fmod(state.theta + pi*2.f, pi*2.f);

    dthdt.push(dTheta / dt);
    dsdt.push(dDist / dt);

    // Determine which coordinates correspond to the sides of the robot
    float dummy = 0.f;
    float* side = &dummy;
    float* front = &dummy;
    float sideDir = 0.f;
    float frontDir = 0.f;
    float thoffset = 0.f;
    const float thwidth2 = pi/12.f;
    if (circleDist(state.theta, 0.f) < thwidth2)
    {
        side = &state.y; sideDir = -1.f;
        front = &state.x; frontDir = 1.f;
        thoffset = state.theta < pi ? 0 : 2.f*pi;
    }
    else if (circleDist(state.theta, pi*0.5f) < thwidth2)
    {
        side = &state.x; sideDir = 1.f;
        front = &state.y; frontDir = 1.f;
        thoffset = pi*0.5f;
    }
    else if (circleDist(state.theta, pi) < thwidth2)
    {
        side = &state.y; sideDir = 1.f;
        front = &state.x; frontDir = -1.f;
        thoffset = pi;
    }
    else if (circleDist(state.theta, pi*1.5f) < thwidth2)
    {
        side = &state.x; sideDir = -1.f;
        front = &state.y; frontDir = -1.f;
        thoffset = pi*1.5f;
    }

    // Add distance sensor measurements to state
    const float sideOffset = (cellw - wallw - sensw)/2.f;
    const float frontOffset = (cellw - wallw)/2.f - fsensoff;
    const float rdist = rightSensor.getDistance();
    const float fdist = frontSensor.getDistance();
    const float ldist = leftSensor.getDistance();

    if (rdist < 0.15f && circleDist(state.theta, thoffset) < 0.5f && std::fabs(dsdt) > 0.01)
    {
        const float thMeas = thoffset - limit(drdt/(dsdt - dthdt*(sensw*0.5f + rdist)), 0.25f*pi);
        state.theta = (1.f - ctheta)*state.theta + ctheta*thMeas;
    }
    if (ldist < 0.15f && circleDist(state.theta, thoffset) < 0.5f && std::fabs(dsdt) > 0.01)
    {
        const float thMeas = thoffset + limit(dldt/(dsdt - dthdt*(sensw*0.5f + ldist)), 0.25f*pi);
        state.theta = (1.f - ctheta)*state.theta + ctheta*thMeas;
    }

    if (rdist < 0.15f)
        *side = (1.f - cside)*(*side) + sideDir*cside*(sideOffset - rdist*std::cos(state.theta - thoffset));
    if (ldist < 0.15f)
        *side = (1.f - cside)*(*side) - sideDir*cside*(sideOffset - ldist*std::cos(state.theta - thoffset));
    if (fdist < 0.15f)
        *front = (1.f - cfront)*(*front) + frontDir*cfront*(frontOffset - fdist*std::cos(state.theta - thoffset));

    // Change current cell if robot has moved far enough
    const float hyst = 0.02f;
    if (state.x > cellw*0.5f + hyst && currentCell.i < mazem - 1)
    {
        state.x -= cellw;
        target.x -= cellw;
        ++currentCell.i;
    }
    if (state.y > cellw*0.5f + hyst && currentCell.j < mazen - 1)
    {
        state.y -= cellw;
        target.y -= cellw;
        ++currentCell.j;
    }
    if (state.x < -(cellw*0.5f + hyst) && currentCell.i > 0)
    {
        state.x += cellw;
        target.x += cellw;
        --currentCell.i;
    }
    if (state.y > -(cellw*0.5f + hyst) && currentCell.j > 0)
    {
        state.y += cellw;
        target.y += cellw;
        --currentCell.j;
    }
        
    // Move towards target
    if (std::fabs(target.x - state.x) < 0.02f &&
        std::fabs(target.y - state.y) < 0.02f &&
        std::fabs(target.theta - state.theta) < 0.1f)
    {
        // Don't move if close enough to the target
        rightWheel.setVelocity(0.f);
        leftWheel.setVelocity(0.f);
    }
    else
    {
        const float thp = std::atan2(target.y - state.y, target.x - state.x);
        thgoal = 2.f*thp - target.theta;
        therr = std::fmod(thgoal - state.theta + 3.f*pi, 2.f*pi) - pi;
        thctrl = thetaController.update(therr);

        const float maxSpeed = 0.2f;
        const float slowDownDist = 0.03f;
        const float targetDist = std::sqrt((target.x - state.x)*(target.x - state.x) +
                                           (target.y - state.y)*(target.y - state.y));
        speed = maxSpeed*std::cos(therr);
        speed = speed > 0.f ? speed : 0.f;
        if (targetDist < slowDownDist)
            speed = speed * targetDist / slowDownDist;
        if ((target.x - state.x)*std::cos(target.theta) + (target.y - state.y)*std::sin(target.theta) < 0.f)
            speed = -speed;
        rightWheel.setVelocity(speed + thctrl);
        leftWheel.setVelocity(speed - thctrl);
    }

    // Send commands to wheels
    leftWheel.update();
    rightWheel.update();

    logData();

    rled(0);
}


void sensorLoop()
{
    const float drlast = rightSensor.getDistance();
    const float dflast = frontSensor.getDistance();
    const float dllast = leftSensor.getDistance();
    
    rightSensor.poll();
    frontSensor.poll();
    leftSensor.poll();

    drdt.push(rightSensor.getDistance() - drlast);
    dfdt.push(frontSensor.getDistance() - dflast);
    dldt.push(leftSensor.getDistance() - dllast);
}


float circleDist(float a, float b)
{
    return std::fabs(std::fmod(a - b + 3.f*pi, 2.f*pi) - pi);
}


float limit(float x, float lim)
{
    return x > lim ? lim : (x < -lim ? -lim : x);
}

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

    drdt.setCutoffFreq(dtsensor, 10.f);
    dfdt.setCutoffFreq(dtsensor, 10.f);
    dldt.setCutoffFreq(dtsensor, 10.f);
    dthdt.setCutoffFreq(dt, 30.f);
    dthdt.setCutoffFreq(dt, 30.f);

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

    x += dDist * std::cos(theta + dTheta * 0.5f);
    y += dDist * std::sin(theta + dTheta * 0.5f);
    theta += dTheta;
    theta = std::fmod(theta + pi*2.f, pi*2.f);

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
    if (theta > pi*2.f - thwidth2 || theta < pi*0.f + thwidth2)
    {
        side = &y; sideDir = -1.f;
        front = &x; frontDir = 1.f;
        thoffset = 0.f;
    }
    else if (theta > pi*0.5f - thwidth2 && theta < pi*0.5f + thwidth2)
    {
        side = &x; sideDir = 1.f;
        front = &y; frontDir = 1.f;
        thoffset = pi*0.5f;
    }
    else if (theta > pi*1.f - thwidth2 && theta < pi*1.f + thwidth2)
    {
        side = &y; sideDir = 1.f;
        front = &x; frontDir = -1.f;
        thoffset = pi;
    }
    else if (theta > pi*1.5f - thwidth2 && theta < pi*1.5f + thwidth2)
    {
        side = &x; sideDir = -1.f;
        front = &y; frontDir = -1.f;
        thoffset = pi*1.5f;
    }

    // Add distance sensor measurements to state
    const float ctheta = 0.01f;
    const float cside = 0.01f;
    const float cfront = 0.01f;
    const float sideOffset = (0.18f - 0.012f - 0.067f)/2.f;
    const float frontOffset = (0.18f - 0.012f - 0.054f)/2.f;

    if (drdt > 2.f*dthdt && std::fabs(theta) < 0.5f)
        theta = (1.f - ctheta)*theta + ctheta*std::atan(drdt / dsdt);
    if (dldt > 2.f*dthdt && std::fabs(theta) < 0.5f)
        theta = (1.f - ctheta) * theta - ctheta * std::atan(dldt / dsdt);

    if (rightSensor.getDistance() < 0.1f)
        *side = (1.f - cside)*(*side) + sideDir*cside*(sideOffset - rightSensor.getDistance()*std::cos(theta - thoffset));
    if (leftSensor.getDistance() < 0.1f)
        *side = (1.f - cside)*(*side) - sideDir*cside*(sideOffset - leftSensor.getDistance()*std::cos(theta - thoffset));
    if (frontSensor.getDistance() < 0.1f)
        *front = (1.f - cfront)*(*front) + frontDir*cfront*(frontOffset - frontSensor.getDistance()*std::cos(theta - thoffset));

    // Send commands to wheels
    leftWheel.update();
    rightWheel.update();

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




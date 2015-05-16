#include "Wheel.h"
#include <Arduino.h>


Wheel::Wheel(int motPin1, int motPin2, int encPin1, int encPin2, float dt, float circumference, int cpr) :
    velocityLoop(dt),
    positionLoop(dt),
    motor(motPin1, motPin2),
    encoder(encPin1, encPin2),
    lastCount(0),
    positionMode(false),
    velocitySetpoint(0.f),
    dt(dt),
    count2dist(circumference / cpr)
{
    velocity.setCutoffFreq(10.f, dt);
    acceleration.setCutoffFreq(10.f, dt);
    velocityKd = 0.3f;
}

void Wheel::update()
{
    const int count = encoder.read();
    
    const float position = count * count2dist;
    const float positionControl = positionLoop.update(positionSetpoint - position);
    
    if (positionMode)
        velocitySetpoint = positionControl;

    const float lastVelocity = velocity;
    velocity.push( (count - lastCount) * count2dist / dt );
    acceleration.push((velocity - lastVelocity) / dt);
    velocityControl = velocityLoop.update(velocitySetpoint - velocity, -acceleration*velocityKd);

    if (velocitySetpoint == 0.f)
        velocityLoop.zeroIntegral();
    
    motor = velocityControl;
    
    lastCount = count;
}


void Wheel::setVelocity(float vel)
{
    velocitySetpoint = vel;
    positionMode = false;
}


float Wheel::getVelocity()
{
    return velocity;
}


void Wheel::setPosition(float pos)
{
    positionSetpoint = pos;
    positionMode = true;
}


float Wheel::getPosition()
{
    return encoder.read() * count2dist;
}


void Wheel::zeroPosition()
{
    encoder.write(0);
}


int Wheel::getCounts()
{
    return encoder.read();
}

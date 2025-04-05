#pragma once

#include <Arduino.h>
#include "Utils.h"



class State
{
public:
    virtual void init() = 0;
    virtual void loop() = 0;
};

class InitializationState : public State
{
public:
    void loop() override;
    void init() override;
};

class SleepState : public State
{
public:
    void loop() override;
    void init() override;

private:
    Vector3 target = Vector3(0, 120, -20);
    bool targetReached = false;
};

class StandingState : public State
{
public:
    void loop() override;
    void init() override;
    void set3HighestLeg();

private:
};


extern State *currentState;
extern State *previousState;

extern InitializationState *initializationState;
extern SleepState *sleepState;
extern StandingState *standingState;
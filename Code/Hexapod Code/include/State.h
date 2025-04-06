#pragma once

#include <Arduino.h>
#include "Utils.h"



class State
{
public:
    virtual void init() = 0;
    virtual void loop() = 0;
    virtual void exit() = 0;
};

class InitializationState : public State
{
public:
    void loop() override;
    void init() override;
    void exit() override;
};

class CalibrationState : public State
{
public:
    void loop() override;
    void init() override;
    void exit() override;
};

class SleepState : public State
{
public:
    void loop() override;
    void init() override;
    void exit() override;

private:
    Vector3 target = Vector3(0, 120, -20);
    bool targetReached = false;
};

class StandingState : public State
{
public:
    void loop() override;
    void init() override;
    void exit() override;
    void set3HighestLeg();

private:
    int points = 1000;
};

class WalkingState : public State
{
public:
    void loop() override;
    void init() override;
    void exit() override;
    Vector3 getGaitPoint(int leg, float pushFraction);

private:
    int points = 1000;
};


extern State *currentState;
extern State *previousState;

extern InitializationState *initializationState;
extern CalibrationState *calibrationState;
extern SleepState *sleepState;
extern StandingState *standingState;
extern WalkingState *walkingState;
#pragma once

#include <PWMServo.h>
#include "Utils.h"
#include "Arduino.h"


#define NUM_LEGS 6

const unsigned long loopFrequency = 100; // Hz (adjust as needed)
const unsigned long loopPeriod = 1000000 / loopFrequency; // microseconds

const int COXA_PINS[NUM_LEGS] = {15, 28, 36, 4, 7, 10};
const int FEMUR_PINS[NUM_LEGS] = {14, 25, 33, 3, 6, 9};
const int TIBIA_PINS[NUM_LEGS] = {29, 37, 24, 2, 5, 8};

const float coxaLength = 45;  // coxa length
const float femurLength = 100;  // femur length
const float tibiaLength = 175; // tibia length
const float totalLegLength = coxaLength + femurLength + tibiaLength;

const float COXA_BASE_OFFSET = 0;
const float FEMUR_BASE_OFFSET = 50;
const float TIBIA_BASE_OFFSET = 0;

const float strideMultiplier[6] = {-1, -1, -1, 1, 1, 1};
const float rotationMultiplier[6] = {1, 0, -1, -1, 0 , 1};

const float distanceFromCenter = 173;
const float distanceFromHexapodBottomToFemurJoint = 40;
const float maxDistanceFromGround = 200;

const Vector3 baseLegCalibrationPosition = Vector3(0, coxaLength, femurLength + tibiaLength);

const int TIME_TO_STAND = 500;
const int TIME_TO_SLEEP = 6000;
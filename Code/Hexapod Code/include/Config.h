#pragma once

#include <PWMServo.h>
#include "Utils.h"
#include "Arduino.h"


#define NUM_LEGS 6

const unsigned long loopFrequency = 200; // Hz (adjust as needed)
const unsigned long loopPeriod = 1000000 / loopFrequency; // microseconds

const int COXA_PINS[NUM_LEGS] = {15, 28, 36, 10, 7, 4};
const int FEMUR_PINS[NUM_LEGS] = {14, 25, 33, 9, 6, 3};
const int TIBIA_PINS[NUM_LEGS] = {29, 37, 24, 8, 5, 2};

const float coxaLength = 45;  // coxa length
const float femurLength = 100;  // femur length
const float tibiaLength = 175; // tibia length
const float totalLegLength = coxaLength + femurLength + tibiaLength;

const float COXA_BASE_OFFSET = 0;
const float FEMUR_BASE_OFFSET = 50;
const float TIBIA_BASE_OFFSET = 0;

const float strideMultiplier[6] = {-1, -1, -1, 1, 1, 1};
const float rotationMultiplier[6] = {1, 0, -1, 1, 0 , -1};

const float distanceFromCenter = 140;
const float distanceFromHexapodBottomToFemurJoint = 40;
const float maxDistanceFromGround = 200;
const float legLandHeight = 45;
const float legPlacementAngle = 55;

const float globalSpeedMult = 0.25;
const float globalRotationStrideLengthMult = 0.8;
const float globalStrafeStrideLengthMult = 0.7;
const float globalLiftHeightMult = 0.85;

const Vector3 baseLegCalibrationPosition = Vector3(0, coxaLength, femurLength + tibiaLength);

const int TIME_TO_STAND = 200;
const int TIME_TO_SLEEP = 12000;
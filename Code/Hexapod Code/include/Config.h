#pragma once

#include <PWMServo.h>
#include "Utils.h"
#include "Arduino.h"


#define NUM_LEGS 6
const int COXA_PINS[NUM_LEGS] = {10, 7, 4, 36, 28, 15};
const int FEMUR_PINS[NUM_LEGS] = {9, 6, 3, 33, 25, 14};
const int TIBIA_PINS[NUM_LEGS] = {8, 5, 2, 24, 37, 29};

const float coxaLength = 45;  // coxa length
const float femurLength = 100;  // femur length
const float tibiaLength = 175; // tibia length
const float totalLegLength = coxaLength + femurLength + tibiaLength;

const float COXA_BASE_OFFSET = 0;
const float FEMUR_BASE_OFFSET = 50;
const float TIBIA_BASE_OFFSET = 0;

const float strideMultiplier[6] = {1, 1, 1, -1, -1, -1};
const float rotationMultiplier[6] = {-1, 0, 1, -1, 0 , 1};

const float distanceFromHexapodBottomToFemurJoint = 40;
const float maxDistanceFromGround = 200;

const Vector3 baseLegConfigurationPosition = Vector3(0, 220, 100);
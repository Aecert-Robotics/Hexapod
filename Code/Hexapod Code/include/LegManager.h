#pragma once

#include <PWMServo.h>
#include "Utils.h"
#include "Arduino.h"
#include "Config.h"

extern Vector3 currentLegPositions[NUM_LEGS];
extern Vector3 saved_offsets[NUM_LEGS];
//extern Vector3 cycleStartPositions[NUM_LEGS];


Vector3 posToAngle(Vector3 pos);
void setLegAngles(int leg, Vector3 angles);
void moveToPos(int leg, Vector3 pos);
void attachServos();

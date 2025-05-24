#pragma once

#include <PWMServo.h>
#include "Utils.h"
#include "Arduino.h"
#include "Config.h"

extern Vector3 currentLegPositions[NUM_LEGS];
extern int8_t calibrationOffsets[18];
//extern Vector3 cycleStartPositions[NUM_LEGS];

enum LegState
{
  Propelling,
  Lifting,
  Standing,
  Reset
};


Vector3 posToAngle(Vector3 pos);
void setLegAngles(int leg, Vector3 angles);
void moveToPos(int leg, Vector3 pos);
void attachServos();
void saveCalibrationOffsets();
void loadCalibrationOffsets();
void printCalibrationOffsets();
Vector3 convertLocalLegPointToGlobal(Vector3 localLegPoint, int legIndex);
Vector3 convertGlobalLegPointToLocal(Vector3 globalLegPoint, int legIndex);

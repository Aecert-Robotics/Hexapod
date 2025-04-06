#pragma once
#include "Arduino.h"
#include "LegManager.h"
#include "NRF.h"
#include "Config.h"

extern float distanceFromGround;
extern bool gyroTilt;
extern Gait currentGait;
extern Gait previousGait;

void updateRuntimeVariables();
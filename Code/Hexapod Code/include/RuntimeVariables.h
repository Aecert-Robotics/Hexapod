#pragma once
#include "Arduino.h"
#include "LegManager.h"
#include "NRF.h"
#include "Config.h"
#include "Gaits.h"

extern float distanceFromGround;
extern bool gyroTilt;
extern Gait cGait; // Current Gait Data Object
void updateRuntimeVariables();
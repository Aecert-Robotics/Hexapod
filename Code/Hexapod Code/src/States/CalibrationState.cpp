
#include <Arduino.h>
#include "State.h"
#include "LegManager.h"
#include "Config.h"
#include "NRF.h"
#include <EEPROM.h>

bool canIncrement = true;
bool canDecrement = true;

void CalibrationState::init()
{
  Serial.println("Calibration State.");
}

void CalibrationState::exit()
{
  Serial.println("Exiting Calibration State.");
  saveCalibrationOffsets();
}

void CalibrationState::loop()
{
  // move legs
  for (int i = 0; i < 6; i++)
  {
    Vector3 next = lerp(currentLegPositions[i], baseLegCalibrationPosition, 0.05);
    //snap to the target position if close enough
    if (abs(currentLegPositions[i].x - baseLegCalibrationPosition.x) < 1)
      next.x = baseLegCalibrationPosition.x;
    
    if (abs(currentLegPositions[i].y - baseLegCalibrationPosition.y) < 1)
      next.y = baseLegCalibrationPosition.y;

    if (abs(currentLegPositions[i].z - baseLegCalibrationPosition.z) < 1)
      next.z = baseLegCalibrationPosition.z;

    moveToPos(i, next);
  }

  // calibrate
  if (rc_settings_data.calibrationIndex == -1)
  {
    return;
  }

  
  if (rc_settings_data.increaseValue == PRESSED && canIncrement)
  {
    calibrationOffsets[rc_settings_data.calibrationIndex] += 1;
    Serial.println("+1");
    canIncrement = false;
  }

  else if (rc_settings_data.decreaseValue == PRESSED && canDecrement)
  {
    calibrationOffsets[rc_settings_data.calibrationIndex] -= 1;
    Serial.println("-1");
    canDecrement = false;
  }

  else if (rc_settings_data.increaseValue == UNPRESSED && !canIncrement)
  {
    canIncrement = true;
  }

  else if (rc_settings_data.decreaseValue == UNPRESSED && !canDecrement)
  {
    canDecrement = true;
  }

  //update the hexapod settings data package with the rawOffsets
  for (int i = 0; i < 18; i++)
  {
    hex_settings_data.calibrationOffsets[i] = calibrationOffsets[i];
  }
}



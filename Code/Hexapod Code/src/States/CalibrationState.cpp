
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

  for (int i = 0; i < 6; i++)
  {
    moveToPos(i, baseLegCalibrationPosition);
  }
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
    float nextX = min(currentLegPositions[i].x + 5, baseLegCalibrationPosition.x);
    float nextY = min(currentLegPositions[i].y + 5, baseLegCalibrationPosition.y);
    float nextZ = min(currentLegPositions[i].z + 5, baseLegCalibrationPosition.z);
    moveToPos(i, Vector3(nextX, nextY, nextZ));
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



#include <Arduino.h>
#include "Utils.h"
#include "LegManager.h"
#include "State.h"
#include "Config.h"
#include "NRF.h"
#include "RuntimeVariables.h"

InitializationState *initializationState = new InitializationState();
CalibrationState *calibrationState = new CalibrationState();
SleepState *sleepState = new SleepState();
StandingState *standingState = new StandingState();
WalkingState *walkingState = new WalkingState();



State *currentState = nullptr;
State *previousState = nullptr;

unsigned long previousLoopTime = 0;

void setup()
{
  Serial.begin(9600);
  attachServos();
  setupNRF();
  loadCalibrationOffsets();

  currentState = initializationState;
  previousState = initializationState;
  
  currentState->init();
  currentState = sleepState;
}

void loop()
{
  unsigned long currentLoopTime = micros();
  unsigned long loopElapsedTime = currentLoopTime - previousLoopTime;

  // Return if the loop has not elapsed enough time
  if (loopElapsedTime < loopPeriod)
    return;

  previousLoopTime = currentLoopTime;

  bool connected = receiveNRFData();
  updateRuntimeVariables();

  // If the state has changed, call the init function of the new state
  if (currentState != previousState)
  {
    previousState->exit();
    currentState->init();
    previousState = currentState;
  }
  currentState->loop();



  //-----------------State Manager---------------------//

  //controller is not connected
  if (!connected)
  {
    currentState = sleepState;
    return;
  }


  // controller is on the homepage
  if (receiveType == RC_CONTROL_DATA)
  {
    static unsigned long lastMovementTime = 0;

    double joy1x = map(rc_control_data.joyLeft_X, 0, 254, -100, 100);
    double joy1y = map(rc_control_data.joyLeft_Y, 0, 254, -100, 100);

    double joy2x = map(rc_control_data.joyRight_X, 0, 254, -100, 100);
    double joy2y = map(rc_control_data.joyRight_Y, 0, 254, -100, 100);

    bool movementDetected = abs(joy1x) > 10 || abs(joy1y) > 10 || abs(joy2x) > 10 || abs(joy2y) > 10;

    if (movementDetected)
    {
      lastMovementTime = millis();
      if (currentState == sleepState)
        currentState = standingState;
      else if (currentState != walkingState)
        currentState = walkingState;
    }
    else
    {
      unsigned long timeSinceLastMovement = millis() - lastMovementTime;
      if (currentState != sleepState && timeSinceLastMovement >= TIME_TO_STAND && timeSinceLastMovement < TIME_TO_SLEEP)
      {
        currentState = standingState;
      }
      else if (timeSinceLastMovement >= TIME_TO_SLEEP && currentState != sleepState)
      {
        currentState = sleepState;
      }
    }
  }
  

  // controller is in the menu
  if (receiveType == RC_SETTINGS_DATA)
  {
    // go to sleep state if the hexapod was not in either calibration or sleep state
    if(currentState != calibrationState && currentState != sleepState) currentState = sleepState;


    // controller has entered the calibration menu
    if (rc_settings_data.calibrating == 1) currentState = calibrationState; 

    // finished calibrating.
    if (currentState == calibrationState && rc_settings_data.calibrating == 0) currentState = sleepState;
  }
}

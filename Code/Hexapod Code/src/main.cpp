#include <Arduino.h>
#include "Utils.h"
#include "LegManager.h"
#include "State.h"
#include "Config.h"
#include "NRF.h"
#include "RuntimeVariables.h"

InitializationState *initializationState = new InitializationState();
SleepState *sleepState = new SleepState();
StandingState *standingState = new StandingState();
WalkingState *walkingState = new WalkingState();

State *currentState = nullptr;
State *previousState = nullptr;

unsigned long previousLoopTime = 0;

void setup() {
  Serial.begin(9600);
  attachServos();
  setupNRF();

  currentState = initializationState;
  currentState->init();

  currentState = standingState;
}

void loop() {
  unsigned long currentLoopTime = micros();
  unsigned long loopElapsedTime = currentLoopTime - previousLoopTime;

  // Return if the loop has not elapsed enough time
  if (loopElapsedTime < loopPeriod)return;


  previousLoopTime = currentLoopTime;

  receiveNRFData();
  updateRuntimeVariables();

  //If the state has changed, call the init function of the new state
  if (currentState != previousState) {
    currentState->init();
    previousState = currentState;    
  }
  currentState->loop();


  // state manager
  static unsigned long lastMovementTime = 0;

  double joy1x = map(rc_data.joyLeft_X, 0, 254, -100, 100);
  double joy1y = map(rc_data.joyLeft_Y, 0, 254, -100, 100);

  double joy2x = map(rc_data.joyRight_X, 0, 254, -100, 100);
  double joy2y = map(rc_data.joyRight_Y, 0, 254, -100, 100);

  bool movementDetected = abs(joy1x) > 10 || abs(joy1y) > 10 || abs(joy2x) > 10 || abs(joy2y) > 10;

  if (movementDetected) {
    lastMovementTime = millis();
    if (currentState == sleepState) {
      currentState = standingState;
    } else if (currentState != walkingState) {
      currentState = walkingState;
    }
  } else {
    unsigned long timeSinceLastMovement = millis() - lastMovementTime;
    if (currentState != sleepState && timeSinceLastMovement >= 3000 && timeSinceLastMovement < 6000) {
      currentState = standingState;
    } else if (timeSinceLastMovement >= 6000 && currentState != sleepState) {
      currentState = sleepState;
    }
  }
}

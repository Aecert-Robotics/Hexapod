#include <Arduino.h>
#include "Utils.h"
#include "LegManager.h"
#include "State.h"
#include "Config.h"
#include "NRF.h"

InitializationState *initializationState = new InitializationState();
SleepState *sleepState = new SleepState();
StandingState *standingState = new StandingState();

State *currentState = nullptr;
State *previousState = nullptr;

void setup() {
  Serial.begin(9600);
  attachServos();
  setupNRF();

  currentState = initializationState;
  currentState->init();

  currentState = sleepState;
}

void loop() {  
  receiveNRFData();
  if (currentState != previousState) {
    previousState = currentState;
    currentState->init();
  }
  currentState->loop();
  

  if(!rc_data.button_A) {
    currentState = standingState;
  } else if (!rc_data.button_B) {
    currentState = sleepState;
  }
}

#include <Arduino.h>
#include "Utils.h"
#include "LegManager.h"
#include "State.h"
#include "Config.h"

State *currentState = nullptr;
State *previousState = nullptr;

InitializationState *initializationState = new InitializationState();
SleepState *sleepState = new SleepState();

void setup() {
  Serial.begin(9600);
  attachServos();
  currentState = initializationState;
  currentState->init();

  currentState = sleepState;
}

void loop() {  
  if (currentState != previousState) {
    previousState = currentState;
    currentState->init();
  }
  currentState->loop();
}

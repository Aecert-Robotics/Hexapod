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

State *currentState = nullptr;
State *previousState = nullptr;

const unsigned long loopFrequency = 100; // Hz (adjust as needed)
const unsigned long loopPeriod = 1000000 / loopFrequency; // microseconds

unsigned long previousLoopTime = 0;

void setup() {
  Serial.begin(9600);
  attachServos();
  setupNRF();

  currentState = initializationState;
  currentState->init();

  currentState = sleepState;
}

void loop() {
  unsigned long currentLoopTime = micros();
  unsigned long loopElapsedTime = currentLoopTime - previousLoopTime;

  if (loopElapsedTime >= loopPeriod) {
    previousLoopTime = currentLoopTime;

    receiveNRFData();
    updateRuntimeVariables();

    if (currentState != previousState) {
      previousState = currentState;
      currentState->init();
    }
    currentState->loop();


    if (!rc_data.button_A) {
      currentState = standingState;
    } else if (!rc_data.button_B) {
      currentState = sleepState;
    }
  }
}


#include <Arduino.h>
#include "State.h"
#include "LegManager.h"
#include "Config.h"


void InitializationState::init(){
  Serial.println("Initialization State."); 
  
  
    for (int i = 0; i < 6; i++) {
      moveToPos(i, baseLegCalibrationPosition);
    }  
    delay(1000);
    
  }
  
  void InitializationState::exit(){
    Serial.println("Exiting Initialization State.");
  }

  void InitializationState::loop(){

  }

#include <Arduino.h>
#include "State.h"
#include "LegManager.h"


void InitializationState::init(){
  Serial.println("Initialization State."); 

    moveToPos(0, Vector3(0,0,160));
  
    moveToPos(1, Vector3(0,0,160));
    moveToPos(2, Vector3(0,0,160));
    moveToPos(3, Vector3(0,0,160));
    moveToPos(4, Vector3(0,0,160));
    moveToPos(5, Vector3(0,0,160));
  
  
    delay(25);
  
  
  
    moveToPos(0, Vector3(0,225,115));
    moveToPos(1, Vector3(0,225,115));
  
    moveToPos(2, Vector3(0,225,115));
    moveToPos(3, Vector3(0,225,115));
    moveToPos(4, Vector3(0,225,115));
    moveToPos(5, Vector3(0,225,115));
    //return;
  
    delay(1000);
  }

  void InitializationState::loop(){

  }
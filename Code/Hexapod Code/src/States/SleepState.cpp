
#include <Arduino.h>
#include "State.h"
#include "LegManager.h"
#include "NRF.h"

void SleepState::init(){
  Serial.println("Sleep State.");
  targetReached = false;  
}

void SleepState::exit(){
  Serial.println("Exiting Sleep State.");
}



void SleepState::loop(){
  //do nothing if all legs have reached the target position
  if(targetReached) return;

  //target = Vector3(0, rc_data.potLeft + 100, rc_data.potRight-50);
  //Serial.println("Target: " + target.toString());

  targetReached = true;
  for (int i = 0; i < 6; i++) {
    Vector3 next = lerp(currentLegPositions[i], target, 0.03);

    //snap to the target position if close enough
    if(abs(currentLegPositions[i].x - target.x) < 1) next.x = target.x;
    if(abs(currentLegPositions[i].y - target.y) < 1) next.y = target.y;
    if(abs(currentLegPositions[i].z - target.z) < 1) next.z = target.z;

    moveToPos(i, next);    

    //if any of the legs havent reached the target, set targetReached to false
    if(currentLegPositions[i] != target)targetReached = false;
  }  
  
}
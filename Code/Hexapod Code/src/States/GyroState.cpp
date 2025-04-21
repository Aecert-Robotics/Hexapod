
#include <Arduino.h>
#include "State.h"
#include "LegManager.h"
#include "NRF.h"

Vector2 joy1TargetVector;
Vector2 joy1CurrentVector;

Vector2 joy2TargetVector;
Vector2 joy2CurrentVector;

Vector3 basePoints[6];

void GyroState::init(){
  Serial.println("Gyro State.");
  for (int i = 0; i < 6; i++)
  {
    basePoints[i] = currentLegPositions[i];
  }
}

void GyroState::exit(){
  Serial.println("Exiting Gyro State.");
}

void GyroState::loop(){
  double joy1x = map(rc_control_data.joyLeft_X, 0, 254, -100, 100);
  double joy1y = map(rc_control_data.joyLeft_Y, 0, 254, -100, 100);

  double joy2x = map(rc_control_data.joyRight_X, 0, 254, -100, 100);
  double joy2y = map(rc_control_data.joyRight_Y, 0, 254, -100, 100);

  joy1TargetVector = Vector2(joy1x, joy1y);
  joy2TargetVector = Vector2(joy2x, joy2y);

  joy1CurrentVector = lerp(joy1CurrentVector, joy1TargetVector, 0.06);
  joy2CurrentVector = lerp(joy2CurrentVector, joy2TargetVector, 0.08);


    Vector3 endPoint;
    for (int i = 0; i < 6; i++) {
        endPoint = basePoints[i];
        //Translate based on the left joystick



        //Rotate based on the right joystick

        

        //Tilt based on the gyro data



        moveToPos(i, endPoint);
    }
  
}
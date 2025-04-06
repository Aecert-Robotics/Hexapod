#include <Arduino.h>
#include "State.h"
#include "LegManager.h"
#include "Config.h"
#include "Utils.h"
#include "NRF.h"
#include "RuntimeVariables.h"

float forwardAmount;
float turnAmount;
float tArray[6];
int ControlPointsAmount = 0;
int RotateControlPointsAmount = 0;
float pushFraction = 3.0 / 6.0;
float speedMultiplier = 0.5;
float strideLengthMultiplier = 1.2; 

float liftHeight = 100;
float liftHeightMultiplier = 1;

float maxStrideLength = 190;
float maxSpeed = 100;

const float legPlacementAngle = 55;
float landHeight = 70;

int leftSlider = 50;
float globalSpeedMultiplier = 0.15;
float globalRotationMultiplier = 0.7;
float globalStrafeMultiplier = 0.7;

Vector2 joy1TargetVector;
float joy1TargetMagnitude;

Vector2 joy2TargetVector;
float joy2TargetMagnitude;

Vector2 joy1CurrentVector;
float joy1CurrentMagnitude;

Vector2 joy2CurrentVector;
float joy2CurrentMagnitude;

int cycleProgress[6];
float points = 1000;



LegState legStates[6];

Vector3 ControlPoints[10];
Vector3 RotateControlPoints[10];
Vector3 cycleStartPoints[6];



void WalkingState::init()
{
  Serial.println("Walking State.");

  // Initialize Leg States
  for (int i = 0; i < 6; i++)
  {
    legStates[i] = Reset;
  }

  switch (currentGait)
  {
  case TRI:
    cycleProgress[0] = 0;
    cycleProgress[1] = (points / 2);
    cycleProgress[2] = 0;
    cycleProgress[3] = (points / 2);
    cycleProgress[4] = 0;
    cycleProgress[5] = (points / 2);

    pushFraction = 3.1 / 6.0;
    speedMultiplier = 1.1;
    strideLengthMultiplier = 1.1;
    liftHeight = 100;
    maxStrideLength = 180;
    maxSpeed = 220;
    break;

  case WAVE:
    // Offsets
    cycleProgress[0] = 0;
    cycleProgress[1] = (points / 6);
    cycleProgress[2] = (points / 6) * 2;
    cycleProgress[3] = (points / 6) * 5;
    cycleProgress[4] = (points / 6) * 4;
    cycleProgress[5] = (points / 6) * 3;

    // Percentage Time On Ground
    pushFraction = 4.9 / 6.0;

    speedMultiplier = 0.40;
    strideLengthMultiplier = 2;
    liftHeight = 110;
    maxStrideLength = 150;
    maxSpeed = 160;
    break;

  case RIPPLE:
    // Offsets
    cycleProgress[0] = 0;
    cycleProgress[1] = (points / 6) * 4;
    cycleProgress[2] = (points / 6) * 2;
    cycleProgress[3] = (points / 6) * 5;
    cycleProgress[4] = (points / 6);
    cycleProgress[5] = (points / 6) * 3;

    // Percentage Time On Ground
    pushFraction = 3.2 / 6.0;

    speedMultiplier = 1;
    strideLengthMultiplier = 1.3;
    liftHeight = 100;
    maxStrideLength = 220;
    maxSpeed = 200;
    break;

  case BI:
    // Offsets
    cycleProgress[0] = 0;
    cycleProgress[1] = (points / 3);
    cycleProgress[2] = (points / 3) * 2;
    cycleProgress[3] = 0;
    cycleProgress[4] = (points / 3);
    cycleProgress[5] = (points / 3) * 2;

    // Percentage Time On Ground
    pushFraction = 2.1 / 6.0;

    speedMultiplier = 4;
    strideLengthMultiplier = 1;
    liftHeight = 160;
    maxStrideLength = 230;
    maxSpeed = 130;
    break;

  case QUAD:
    // Offsets
    cycleProgress[0] = 0;
    cycleProgress[1] = (points / 3);
    cycleProgress[2] = (points / 3) * 2;
    cycleProgress[3] = 0;
    cycleProgress[4] = (points / 3);
    cycleProgress[5] = (points / 3) * 2;

    // Percentage Time On Ground
    pushFraction = 4.1 / 6.0;

    speedMultiplier = 1;
    strideLengthMultiplier = 1.2;
    liftHeight = 100;
    maxStrideLength = 220;
    maxSpeed = 200;
    break;

  case HOP:
    // Offsets
    cycleProgress[0] = 0;
    cycleProgress[1] = 0;
    cycleProgress[2] = 0;
    cycleProgress[3] = 0;
    cycleProgress[4] = 0;
    cycleProgress[5] = 0;

    // Percentage Time On Ground
    pushFraction = 3 / 6.0;

    speedMultiplier = 1;
    strideLengthMultiplier = 1.6;
    liftHeight = 170;
    maxStrideLength = 240;
    maxSpeed = 200;
    break;
  }
}

void WalkingState::exit()
{
  Serial.println("Exiting Walking State.");
}

void WalkingState::loop()
{
  double joy1x = map(rc_control_data.joyLeft_X, 0, 254, -100, 100);
  double joy1y = map(rc_control_data.joyLeft_Y, 0, 254, -100, 100);

  double joy2x = map(rc_control_data.joyRight_X, 0, 254, -100, 100);
  double joy2y = map(rc_control_data.joyRight_Y, 0, 254, -100, 100);

  joy1TargetVector = Vector2(joy1x, joy1y);
  joy1TargetMagnitude = constrain(calculateHypotenuse(abs(joy1x), abs(joy1y)), 0, 100);

  joy2TargetVector = Vector2(joy2x, joy2y);
  joy2TargetMagnitude = constrain(calculateHypotenuse(abs(joy2x), abs(joy2y)), 0, 100);

  joy1CurrentVector = lerp(joy1CurrentVector, joy1TargetVector, 0.04);
  joy1CurrentMagnitude = lerp(joy1CurrentMagnitude, joy1TargetMagnitude, 0.04);

  joy2CurrentVector = lerp(joy2CurrentVector, joy2TargetVector, 0.06);
  joy2CurrentMagnitude = lerp(joy2CurrentMagnitude, joy2TargetMagnitude, 0.06);



  for (int i = 0; i < 6; i++)
  {
    tArray[i] = (float)cycleProgress[i] / points;
  }

  forwardAmount = joy1CurrentMagnitude;
  turnAmount = joy2CurrentVector.x;

  

  moveToPos(0, getGaitPoint(0, pushFraction));
  moveToPos(1, getGaitPoint(1, pushFraction));
  moveToPos(2, getGaitPoint(2, pushFraction));
  moveToPos(3, getGaitPoint(3, pushFraction));
  moveToPos(4, getGaitPoint(4, pushFraction));
  moveToPos(5, getGaitPoint(5, pushFraction));

  //print_value("leg 5 walking point", currentLegPositions[4], true);

  float progressChangeAmount = (max(abs(forwardAmount), abs(turnAmount)) * speedMultiplier) * globalSpeedMultiplier;

  progressChangeAmount = constrain(progressChangeAmount, 0, maxSpeed * globalSpeedMultiplier);

  for (int i = 0; i < 6; i++)
  {
    cycleProgress[i] += progressChangeAmount;

    if (cycleProgress[i] >= points)
    {
      cycleProgress[i] = cycleProgress[i] - points;
    }
  }
}

Vector3 WalkingState::getGaitPoint(int leg, float pushFraction){  
 

  float rotateStrideLength = joy2CurrentVector.x * globalRotationMultiplier;  
  
  Vector2 strafeStrideLength = joy1CurrentVector * globalStrafeMultiplier * strideLengthMultiplier;
  strafeStrideLength.y = constrain(strafeStrideLength.y,-maxStrideLength/2, maxStrideLength/2);
  strafeStrideLength.x = constrain(strafeStrideLength.x,-maxStrideLength, maxStrideLength);

  float weightSum = abs(forwardAmount) + abs(turnAmount);

  float t = tArray[leg];
  
  
  
  //Propelling
  if(t < pushFraction){ 
    if(legStates[leg] != Propelling) cycleStartPoints[leg] = currentLegPositions[leg];
    legStates[leg] = Propelling;


    //-----This cycle is a straight line that will cause the hexapod to strafe-----//
    ControlPointsAmount = 2;  

    //Starting point of the line
    ControlPoints[0] = cycleStartPoints[leg];

    //Ending point of the line
    ControlPoints[1] = Vector3(
      strafeStrideLength.y * strideMultiplier[leg],                         //X                      
      -strafeStrideLength.x * strideMultiplier[leg] + distanceFromCenter,   //Y
      distanceFromGround                                                    //Z
    ).rotate(legPlacementAngle * rotationMultiplier[leg], Vector2(0,distanceFromCenter));

    Vector3 strafePoint = GetPointOnBezierCurve(ControlPoints, ControlPointsAmount, mapFloat(t,0,pushFraction,0,1));
    //-------------------------------------------------------------------------------//


    //------This cycle is a curved line that will cause the hexapod to rotate in place-----//
    RotateControlPointsAmount = 3;  
    
    //Starting point of the curve
    RotateControlPoints[0] = cycleStartPoints[leg];

    //Middle point of the curve
    RotateControlPoints[1] = Vector3(
      0,                        //X
      distanceFromCenter + 40,  //Y 
      distanceFromGround        //Z
    );

    //Ending point of the curve
    RotateControlPoints[2] = Vector3(
      rotateStrideLength,   //X
      distanceFromCenter,   //Y
      distanceFromGround    //Z
    );
      
    Vector3 rotatePoint = GetPointOnBezierCurve(RotateControlPoints, RotateControlPointsAmount, mapFloat(t,0,pushFraction,0,1));
    //-------------------------------------------------------------------------------//

    //Return the weighted average of the two points
    return (strafePoint*abs(forwardAmount) + rotatePoint*abs(turnAmount))/ weightSum;
  }

  //Lifting
  else{
    if(legStates[leg] != Lifting) cycleStartPoints[leg] = currentLegPositions[leg];
    legStates[leg] = Lifting;

    //------This cycle will cause the hexapod leg to lift up and return to the beginning of the walk cycle in a straight line-----//
    ControlPointsAmount = 4;

    //Starting point of the curve
    ControlPoints[0] = cycleStartPoints[leg];

    //Control point directly above the starting point causing the leg to lift up quickly
    ControlPoints[1] = cycleStartPoints[leg] + Vector3(0,0,liftHeight*liftHeightMultiplier);

    //Control point directly above the ending point preventing the leg from running into the ground
    ControlPoints[2] = Vector3(
      -strafeStrideLength.y * strideMultiplier[leg],                      //X
      strafeStrideLength.x * strideMultiplier[leg] + distanceFromCenter,  //Y
      distanceFromGround + landHeight                                     //Z                          
    ).rotate(legPlacementAngle * rotationMultiplier[leg], Vector2(0,distanceFromCenter));

    //Ending point of the curve
    ControlPoints[3] = Vector3(
      -strafeStrideLength.y * strideMultiplier[leg], 
      strafeStrideLength.x * strideMultiplier[leg] + distanceFromCenter, 
      distanceFromGround
    ).rotate(legPlacementAngle * rotationMultiplier[leg], Vector2(0,distanceFromCenter));
    
    Vector3 straightPoint = GetPointOnBezierCurve(ControlPoints, ControlPointsAmount, mapFloat(t,pushFraction,1,0,1));
    //-------------------------------------------------------------------------------//


    //------This cycle will cause the hexapod leg to lift up and return to the beginning of the walk cycle in a curved line-----//
    RotateControlPointsAmount = 5;

    //Starting point of the curve
    RotateControlPoints[0] = cycleStartPoints[leg];

    //Control point directly above the starting point causing the leg to lift up quickly
    RotateControlPoints[1] = cycleStartPoints[leg] + Vector3(0,0,liftHeight*liftHeightMultiplier);

    //Control point at the apex of the curve and offset away from the hexapods body, cause the leg to lift up and away.
    RotateControlPoints[2] = Vector3(
      0,                              //X
      distanceFromCenter + 40,        //Y
      distanceFromGround + liftHeight*liftHeightMultiplier //Z
    );

    //Control point directly above the ending point preventing the leg from running into the ground
    RotateControlPoints[3] = Vector3(
      -rotateStrideLength,            //X
      distanceFromCenter,             //Y
      distanceFromGround + landHeight //Z
    );

    //Ending point of the curve
    RotateControlPoints[4] = Vector3(
      -rotateStrideLength,            //X
      distanceFromCenter,             //Y
      distanceFromGround               //Z
    );
    
    Vector3 rotatePoint =  GetPointOnBezierCurve(RotateControlPoints, RotateControlPointsAmount, mapFloat(t,pushFraction,1,0,1));
    //-------------------------------------------------------------------------------//

    //Return the weighted average of the two points
    return (straightPoint*abs(forwardAmount) + rotatePoint*abs(turnAmount))/ weightSum;
  }  
}
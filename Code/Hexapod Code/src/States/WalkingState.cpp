#include <Arduino.h>
#include "State.h"
#include "Gaits.h"
#include "LegManager.h"
#include "Config.h"
#include "Utils.h"
#include "NRF.h"
#include "RuntimeVariables.h"

float forwardAmount;
float turnAmount;
float tArray[6];


float landHeight = 70;

float globalSpeedMult = 0.5;
float globalRotationStrideLengthMult = 0.7;
float globalStrafeStrideLengthMult = 0.7;
float globalLiftHeightMult = 1;

Vector2 joy1TargetVector;
float joy1TargetMagnitude;

Vector2 joy1CurrentVector;
float joy1CurrentMagnitude;

Vector2 joy2TargetVector;
float joy2TargetMagnitude;

Vector2 joy2CurrentVector;
float joy2CurrentMagnitude;

int cycleProgress[6];
float points = 1000;

LegState legStates[6];
Vector3 cycleStartPoints[6];



void WalkingState::init()
{
  Serial.println("Walking State.");

  // Initialize Leg States
  for (int i = 0; i < 6; i++)
  {
    legStates[i] = Reset;
    cycleProgress[i] = cGait.cycleOffsetPercentages[i] * points;
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

  float potRightPercentage = mapFloat(rc_control_data.potRight, 0, 254, 0.01, 1);



  for (int i = 0; i < 6; i++)
  {
    tArray[i] = (float)cycleProgress[i] / points;
  }

  forwardAmount = joy1CurrentMagnitude;
  turnAmount = joy2CurrentVector.x;  

  moveToPos(0, getGaitPoint(0, cGait.pushFraction));
  moveToPos(1, getGaitPoint(1, cGait.pushFraction));
  moveToPos(2, getGaitPoint(2, cGait.pushFraction));
  moveToPos(3, getGaitPoint(3, cGait.pushFraction));
  moveToPos(4, getGaitPoint(4, cGait.pushFraction));
  moveToPos(5, getGaitPoint(5, cGait.pushFraction));

  float progressChangeAmount = max(abs(forwardAmount), abs(turnAmount)) * cGait.gaitSpeedMult * globalSpeedMult * potRightPercentage;


  // update the cycle progress for each leg
  for (int i = 0; i < 6; i++)
  {
    cycleProgress[i] += progressChangeAmount;

    // loop the cycle progress if it exceeds the points
    if (cycleProgress[i] >= points) cycleProgress[i] = cycleProgress[i] - points;
  }
}

Vector3 WalkingState::getGaitPoint(int leg, float pushFraction){
  
  float rotateStrideLength = joy2CurrentVector.x * globalRotationStrideLengthMult;  
  
  Vector2 strafeStrideLength = joy1CurrentVector * globalStrafeStrideLengthMult* cGait.strideLengthMult;
  strafeStrideLength.y = constrain(strafeStrideLength.y,-cGait.maxStrideLength/2, cGait.maxStrideLength/2);
  strafeStrideLength.x = constrain(strafeStrideLength.x,-cGait.maxStrideLength, cGait.maxStrideLength);

  float t = tArray[leg];
  
  
  //Propelling
  if(t < pushFraction){ 
    if(legStates[leg] != Propelling) cycleStartPoints[leg] = currentLegPositions[leg];
    legStates[leg] = Propelling;


    //-----This cycle is a straight line that will cause the hexapod to strafe-----//

    //Starting point of the line
    vector<Vector3> strafeControlPoints = vector<Vector3>(2);
    strafeControlPoints[0] = cycleStartPoints[leg];

    //Ending point of the line
    strafeControlPoints[1] = Vector3(
      strafeStrideLength.y * strideMultiplier[leg],                         //X                      
      -strafeStrideLength.x * strideMultiplier[leg] + distanceFromCenter,   //Y
      distanceFromGround                                                    //Z
    ).rotate(legPlacementAngle * rotationMultiplier[leg], Vector2(0,distanceFromCenter));

    Vector3 strafePoint = GetPointOnBezierCurve(strafeControlPoints, mapFloat(t,0,pushFraction,0,1));
    //-------------------------------------------------------------------------------//
    
    //Starting point of the curve
    vector<Vector3> rotateControlPoints = vector<Vector3>(3);
    rotateControlPoints[0] = cycleStartPoints[leg];

    //Middle point of the curve
    rotateControlPoints[1] = Vector3(
      0,                        //X
      distanceFromCenter + 0,  //Y 
      distanceFromGround        //Z
    );

    //Ending point of the curve
    rotateControlPoints[2] = Vector3(
      rotateStrideLength,   //X
      distanceFromCenter,   //Y
      distanceFromGround    //Z
    );
      
    Vector3 rotatePoint = GetPointOnBezierCurve(rotateControlPoints, mapFloat(t,0,pushFraction,0,1));
    //-------------------------------------------------------------------------------//

    //Return the weighted average of the two points
    return (strafePoint*abs(forwardAmount) + rotatePoint*abs(turnAmount))/ (abs(forwardAmount) + abs(turnAmount));
  }

  //Lifting
  else{
    if(legStates[leg] != Lifting) cycleStartPoints[leg] = currentLegPositions[leg];
    legStates[leg] = Lifting;

    //------This cycle will cause the hexapod leg to lift up and return to the beginning of the walk cycle in a straight line-----//

    //Starting point of the curve
    vector<Vector3> strafeControlPoints = vector<Vector3>(4);
    strafeControlPoints[0] = cycleStartPoints[leg];

    //Control point directly above the starting point causing the leg to lift up quickly
    strafeControlPoints[1] = cycleStartPoints[leg] + Vector3(0, 0, cGait.liftHeight * globalLiftHeightMult);

    //Control point directly above the ending point preventing the leg from running into the ground
    strafeControlPoints[2] = Vector3(
      -strafeStrideLength.y * strideMultiplier[leg],                      //X
      strafeStrideLength.x * strideMultiplier[leg] + distanceFromCenter,  //Y
      distanceFromGround + landHeight                                     //Z                          
    ).rotate(legPlacementAngle * rotationMultiplier[leg], Vector2(0,distanceFromCenter));

    //Ending point of the curve
    strafeControlPoints[3] = Vector3(
      -strafeStrideLength.y * strideMultiplier[leg], 
      strafeStrideLength.x * strideMultiplier[leg] + distanceFromCenter, 
      distanceFromGround
    ).rotate(legPlacementAngle * rotationMultiplier[leg], Vector2(0,distanceFromCenter));
    
    Vector3 straightPoint = GetPointOnBezierCurve(strafeControlPoints, mapFloat(t,pushFraction,1,0,1));
    //-------------------------------------------------------------------------------//


    //------This cycle will cause the hexapod leg to lift up and return to the beginning of the walk cycle in a curved line-----//

    //Starting point of the curve
    vector<Vector3> rotateControlPoints = vector<Vector3>(5);
    rotateControlPoints[0] = cycleStartPoints[leg];

    //Control point directly above the starting point causing the leg to lift up quickly
    rotateControlPoints[1] = cycleStartPoints[leg] + Vector3(0, 0, cGait.liftHeight * globalLiftHeightMult);

    //Control point at the apex of the curve and offset away from the hexapods body, cause the leg to lift up and away.
    rotateControlPoints[2] = Vector3(
      0,                              //X
      distanceFromCenter + 0,        //Y
      distanceFromGround + cGait.liftHeight * globalLiftHeightMult //Z
    );

    //Control point directly above the ending point preventing the leg from running into the ground
    rotateControlPoints[3] = Vector3(
      -rotateStrideLength,            //X
      distanceFromCenter,             //Y
      distanceFromGround + landHeight //Z
    );

    //Ending point of the curve
    rotateControlPoints[4] = Vector3(
      -rotateStrideLength,            //X
      distanceFromCenter,             //Y
      distanceFromGround              //Z
    );
    
    Vector3 rotatePoint =  GetPointOnBezierCurve(rotateControlPoints, mapFloat(t,pushFraction,1,0,1));
    //-------------------------------------------------------------------------------//

    //Return the weighted average of the two points
    return (straightPoint*abs(forwardAmount) + rotatePoint*abs(turnAmount))/ (abs(forwardAmount) + abs(turnAmount));
  }  
}
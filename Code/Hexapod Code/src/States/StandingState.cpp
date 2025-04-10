#include <Arduino.h>
#include "State.h"
#include "LegManager.h"
#include "NRF.h"
#include "RuntimeVariables.h"

// Standing Control Points Array
Vector3 SCPA[6][10];

Vector3 standingStartPoints[6];     // the points the legs are at in the beginning of the standing state
Vector3 standingInBetweenPoints[6]; // the middle points of the bezier curves that the legs will follow to smoothly transition to the end points
Vector3 standingEndPoint;

int currentLegs[3] = {-1, -1, -1};
int standLoops = 0;
int standProgress = 0;

bool moveAllAtOnce = false;
bool highLift = false;

void StandingState::init()
{
  Serial.println("Standing State.");
  if (previousState == sleepState)
    moveAllAtOnce = true;
  set3HighestLeg();
  standLoops = 0;
  standProgress = 0;
  memcpy(standingStartPoints, currentLegPositions, sizeof(currentLegPositions[0]) * 6);

  standingEndPoint = Vector3(0, distanceFromCenter, distanceFromGround);
  //Serial.println("Standing End Point: " + standingEndPoint.toString());

  // Calculate the inbetween and ending points
  for (int i = 0; i < 6; i++)
  {
    Vector3 inBetweenPoint = standingStartPoints[i];
    inBetweenPoint.x = (inBetweenPoint.x + standingEndPoint.x) / 1.5;
    inBetweenPoint.y = (inBetweenPoint.y + standingEndPoint.y) / 1.5;

    inBetweenPoint.z = ((inBetweenPoint.z + standingEndPoint.z) / 2);
    if (abs(inBetweenPoint.z - standingEndPoint.z) < 50)
      inBetweenPoint.z += 70;
    // if(highLift)inBetweenPoint.z += 80;

    standingInBetweenPoints[i] = inBetweenPoint;

    SCPA[i][0] = standingStartPoints[i];
    SCPA[i][1] = standingInBetweenPoints[i];
    SCPA[i][2] = standingEndPoint;
  }

  

  // standing. This takes about a second
  unsigned long previousStandLoopTime = micros(); // Add this line

  while (standLoops < 2)
  {
    unsigned long currentStandLoopTime = micros(); // Add this line
    unsigned long standLoopElapsedTime = currentStandLoopTime - previousStandLoopTime; // Add this line

    if (standLoopElapsedTime < loopPeriod) continue; // Add this line

    previousStandLoopTime = currentStandLoopTime; // Add this line

    standProgress += 20;
    float t = (float)standProgress / points;
    if (t > 1)
    {
      t = 1;
    }

    if (moveAllAtOnce)
    {
      for (int i = 0; i < 6; i++)
      {
        moveToPos(i, GetPointOnBezierCurve(SCPA[i], 3, t));
      }

      if (standProgress > points)
      {
        standProgress = 0;
        standLoops = 2;
      }
    }

    else
    {
      for (int i = 0; i < 3; i++)
      {
        if (currentLegs[i] != -1)
        {
          moveToPos(currentLegs[i], GetPointOnBezierCurve(SCPA[currentLegs[i]], 3, t));
        }
      }

      if (standProgress > points)
      {
        standProgress = 0;
        standLoops++;
        set3HighestLeg();
      }
    }
  }  
}


void StandingState::exit()
{
  Serial.println("Exiting Standing State.");
}


void StandingState::loop()
{
  standingEndPoint = Vector3(0, distanceFromCenter, distanceFromGround);
  //Serial.println("Standing End Point: " + standingEndPoint.toString());

  // update distance from ground constantly
  for (int i = 0; i < 6; i++) SCPA[i][2] = standingEndPoint;

  // constantly move to the standing end position
  for (int i = 0; i < 6; i++) moveToPos(i, GetPointOnBezierCurve(SCPA[i], 3, 1));
  return;
}

void StandingState::set3HighestLeg()
{

  currentLegs[0] = -1;
  currentLegs[1] = -1;
  currentLegs[2] = -1;
  for (int j = 0; j < 3; j++)
  {
    for (int i = 0; i < 6; i++)
    { // go through the legs
      // if the leg is already on the list of current legs, skip it
      if (currentLegs[0] == i || currentLegs[1] == i || currentLegs[2] == i)
        continue;

      // if the leg is already in position, dont add it
      if (currentLegPositions[i] == standingEndPoint)
        continue;

      // if the legs z is greater than the leg already there, add it
      if (currentLegs[j] == -1 || currentLegPositions[i].z > currentLegPositions[currentLegs[j]].z)
      {
        currentLegs[j] = i;
      }
    }
  }
}
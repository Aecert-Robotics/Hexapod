#include <Arduino.h>
#include "State.h"
#include "LegManager.h"
#include "NRF.h"
#include "RuntimeVariables.h"
#include <vector> // Include the vector header

// Standing Control Points Array - Changed to a vector of vectors
vector<vector<Vector3>> SCPA(6, vector<Vector3>(3));

Vector3 standingStartPoints[6];     // the points the legs are at in the beginning of the standing state
Vector3 standingInBetweenPoints[6]; // the middle points of the bezier curves that the legs will follow to smoothly transition to the end points
Vector3 standingEndPoint;

int legAdjustOrder[6] = {1, 2, 3, 4, 5, 6}; // the order in which the legs will be adjusted to stand
int standProgress[6] = {0, 0, 0, 0, 0, 0};
int legAdjustTimeOffset = 120;

bool moveAllAtOnce = false;

void StandingState::init()
{
  Serial.println("Standing State.");

  if (previousState == sleepState){
    moveAllAtOnce = true;
    Serial.println("Moving all at once.");
  }
  else
  {
    moveAllAtOnce = false;
    Serial.println("Moving one at a time.");
  }

  calculateLegAdjustOrder();

  memcpy(standingStartPoints, currentLegPositions, sizeof(currentLegPositions[0]) * 6);
  standingEndPoint = Vector3(0, distanceFromCenter, distanceFromGround);

  // Calculate the inbetween and ending points
  for (int i = 0; i < 6; i++)
  {
    Vector3 inBetweenPoint = standingStartPoints[i];
    inBetweenPoint.x = (inBetweenPoint.x + standingEndPoint.x) / 1.5;
    inBetweenPoint.y = (inBetweenPoint.y + standingEndPoint.y) / 1.5;

    inBetweenPoint.z = ((inBetweenPoint.z + standingEndPoint.z) / 2);
    if (abs(inBetweenPoint.z - standingEndPoint.z) < 50)
      inBetweenPoint.z += 60;

    standingInBetweenPoints[i] = inBetweenPoint;

    SCPA[i][0] = standingStartPoints[i];
    SCPA[i][1] = standingInBetweenPoints[i];
    SCPA[i][2] = standingEndPoint;
  }

  unsigned long standingStartTime = millis();

  bool adjusting = true;
  for (int i = 0; i < 6; i++)
  {
    standProgress[i] = 0;
  }
  while (adjusting)
  {
    // only run every loopPeriod milliseconds
    unsigned long loopPeriodMs = loopPeriod / 1000;
    every(loopPeriodMs)
    {
      unsigned long currentTime = millis();
      // iterate over all legs
      for (int i = 0; i < 6; i++)
      {

        int currentLeg = legAdjustOrder[i];

        // check if the leg is already adjusted
        if (standProgress[currentLeg] >= points)
          continue;

        // moveAllAtOnce == TRUE: adjust all of the legs at the same time
        if (moveAllAtOnce)
        {
          standProgress[currentLeg] += 10;
        }

        // moveAllAtOnce == FALSE: offset the adjustment of the legs by the legAdjustTimeOffset
        else if (currentTime - standingStartTime >= i * legAdjustTimeOffset)
        {
          standProgress[currentLeg] += 30;
        }

        // move to the next point in the bezier curve
        float t = (float)standProgress[currentLeg] / points;
        if (t > 1)
          t = 1;
        moveToPos(currentLeg, GetPointOnBezierCurve(SCPA[currentLeg], t));
      }

      // check if all legs are done moving
      adjusting = false;
      for (int i = 0; i < 6; i++)
      {
        if (standProgress[i] < points)
        {
          adjusting = true;
          break;
        }
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
  // Serial.println("Standing End Point: " + standingEndPoint.toString());

  // update distance from ground constantly
  for (int i = 0; i < 6; i++)
    SCPA[i][2] = standingEndPoint; // Update the end point in the vector

  // constantly move to the standing end position
  // Pass the vector directly, t=1 means the end point
  for (int i = 0; i < 6; i++)
    moveToPos(i, GetPointOnBezierCurve(SCPA[i], 1.0f));
  return;
}

void StandingState::calculateLegAdjustOrder()
{
  // Fill with indices 0..5
  for (int i = 0; i < 6; i++)
  {
    legAdjustOrder[i] = i;
  }

  // Sort indices by z value, highest first
  std::sort(legAdjustOrder, legAdjustOrder + 6, [](int a, int b)
            { return currentLegPositions[a].z > currentLegPositions[b].z; });
}
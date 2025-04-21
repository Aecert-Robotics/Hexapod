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

int legAdjustOrder[6] = {-1, -1, -1, -1, -1, -1}; // the order in which the legs will be adjusted to stand
int standLoops = 0;
int standProgress = 0;

bool moveAllAtOnce = false;
bool highLift = true;

void StandingState::init()
{
  Serial.println("Standing State.");

  if (previousState == sleepState) moveAllAtOnce = true;
  else moveAllAtOnce = false;

  if (moveAllAtOnce)Serial.println("Moving all legs at once.");
  else Serial.println("Moving 1 leg at a time.");

  calculateLegAdjustOrder();

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
    //if(highLift)inBetweenPoint.z += 80;

    standingInBetweenPoints[i] = inBetweenPoint;

    // Resize the inner vector if necessary (should be 3 points: start, middle, end)
    if (SCPA[i].size() != 3) {
        SCPA[i].resize(3);
    }
    SCPA[i][0] = standingStartPoints[i];
    SCPA[i][1] = standingInBetweenPoints[i];
    SCPA[i][2] = standingEndPoint;
  }

  // standing. This takes about a second
  unsigned long previousStandLoopTime = micros(); // Add this line

  while (standLoops < 6)
  {
    unsigned long currentStandLoopTime = micros(); // Add this line
    unsigned long standLoopElapsedTime = currentStandLoopTime - previousStandLoopTime; // Add this line

    if (standLoopElapsedTime < loopPeriod) continue; // Add this line

    previousStandLoopTime = currentStandLoopTime; // Add this line

    standProgress += 20;
    if (!moveAllAtOnce) standProgress += 40;
    float t = (float)standProgress / points;
    if (t > 1)
    {
      t = 1;
    }

    if (moveAllAtOnce)
    {
      
      for (int i = 0; i < 6; i++)
      {
        // Pass the vector directly
        moveToPos(i, GetPointOnBezierCurve(SCPA[i], t));
      }

      if (standProgress > points)
      {
        standProgress = 0;
        break;
      }
    }

    else
    {
      
      moveToPos(legAdjustOrder[standLoops], GetPointOnBezierCurve(SCPA[legAdjustOrder[standLoops]], t));

      if (standProgress > points)
      {
        standProgress = 0;
        standLoops++;
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
  for (int i = 0; i < 6; i++) SCPA[i][2] = standingEndPoint; // Update the end point in the vector

  // constantly move to the standing end position
  // Pass the vector directly, t=1 means the end point
  for (int i = 0; i < 6; i++) moveToPos(i, GetPointOnBezierCurve(SCPA[i], 1.0f));
  return;
}

void StandingState::calculateLegAdjustOrder()
{
    // Fill with indices 0..5
    for (int i = 0; i < 6; i++) {
        legAdjustOrder[i] = i;
    }

    // Sort indices by z value, highest first
    std::sort(legAdjustOrder, legAdjustOrder + 6, [](int a, int b) {
        return currentLegPositions[a].z > currentLegPositions[b].z;
    });

    // Print out the order
    Serial.print("Leg Adjust Order: ");
    for (int i = 0; i < 6; i++) {
        Serial.print(legAdjustOrder[i]);
        Serial.print(" ");
    }
    Serial.println();
}
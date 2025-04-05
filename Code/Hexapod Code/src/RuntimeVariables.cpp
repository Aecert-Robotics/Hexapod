#include "RuntimeVariables.h"
#include "NRF.h"
#include "Config.h"

float distanceFromGround = -100;
bool gyroTilt = false;

const float smoothingFactor = 0.1; // Adjust this value (0.0 to 1.0)

void updateRuntimeVariables() {
    int leftPot = map(rc_control_data.potLeft, 0, 100, 0, -maxDistanceFromGround + distanceFromHexapodBottomToFemurJoint);
    int targetDistanceFromGround = leftPot - distanceFromHexapodBottomToFemurJoint;
    distanceFromGround = distanceFromGround + smoothingFactor * (targetDistanceFromGround - distanceFromGround);
  
    //gyroTilt = tilt;
}
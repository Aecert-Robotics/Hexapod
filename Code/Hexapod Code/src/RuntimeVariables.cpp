#include "RuntimeVariables.h"

float distanceFromGround = -100;
bool gyroTilt = false;
Gait currentGait = TRI;
Gait previousGait = TRI;

const float smoothingFactor = 0.1; // Adjust this value (0.0 to 1.0)

void updateRuntimeVariables() {
    int leftPot = map(rc_control_data.potLeft, 0, 100, 0, -maxDistanceFromGround + distanceFromHexapodBottomToFemurJoint);
    int targetDistanceFromGround = leftPot - distanceFromHexapodBottomToFemurJoint;
    distanceFromGround = distanceFromGround + smoothingFactor * (targetDistanceFromGround - distanceFromGround);

    previousGait = currentGait;
    currentGait = (Gait)rc_control_data.gait;
  
    //gyroTilt = tilt;
}
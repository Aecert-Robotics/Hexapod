#include <Arduino.h>
#include "State.h"
#include "LegManager.h"
#include "NRF.h"
#include "RuntimeVariables.h"

Vector3 footLocal[6]; 
Vector3 footGlobal[6];         // Feet in world coordinates
Vector3 legAttachmentOffsets[6]; // From body center to hip, in body frame

void GyroStateFixed::init()
{
  Serial.println("Gyro State.");
  for (int i = 0; i < 6; i++)
  {
    footLocal[i] = currentLegPositions[i];
    legAttachmentOffsets[i] = convertLocalLegPointToGlobal(Vector3(0,0,0), i); // Center of body
    footGlobal[i] = convertLocalLegPointToGlobal(footLocal[i], i); 
  }

}

void GyroStateFixed::exit()
{
  Serial.println("Exiting Gyro State.");
}

void GyroStateFixed::loop()
{
  // --- Read Inputs & Apply Smoothing ---
  double joy1x = map(rc_control_data.joyLeft_X, 0, 254, -100, 100);
  double joy1y = map(rc_control_data.joyLeft_Y, 0, 254, -100, 100);
  double joy2x = map(rc_control_data.joyRight_X, 0, 254, -100, 100);
  // double joy2y = map(rc_control_data.joyRight_Y, 0, 254, -100, 100); // joy2y not used currently

  double gyroX = constrain(rc_control_data.gyro_X, -45, 45);
  double gyroY = constrain(rc_control_data.gyro_Y, -45, 45);

  joy1TargetVector = Vector2(joy1x, joy1y);
  joy2TargetVector = Vector2(joy2x, 0); // Only using X for yaw

  joy1CurrentVector = lerp(joy1CurrentVector, joy1TargetVector, 0.06);
  joy2CurrentVector = lerp(joy2CurrentVector, joy2TargetVector, 0.08);

  gyroTargetVector = Vector2(-gyroX, -gyroY); // Map gyro to roll/pitch
  gyroCurrentVector = lerp(gyroCurrentVector, gyroTargetVector, 0.1);

  // --- Define Body Orientation ---
  float pitch = gyroCurrentVector.x; // Gyro X mapped to Pitch
  float roll = gyroCurrentVector.y;  // Gyro Y mapped to Roll
  // Yaw control from right joystick X, scaled down
  float yaw = -joy2CurrentVector.x / 2.0;

  // --- Per-Leg Calculation Loop ---
  for (int i = 0; i < 6; i++)
  {
    // --- Part 1: Calculate Desired Foot Position from Joysticks (relative to leg base, local frame) ---

    // Start with the base/neutral local position for this leg
    Vector3 desiredLocalPos = footLocal[i];

    desiredLocalPos.x += joy1CurrentVector.y * strideMultiplier[i];
    desiredLocalPos.y += joy1CurrentVector.x * strideMultiplier[i] * -1;

    // Rotate into body/global frame
    desiredLocalPos = desiredLocalPos.rotate(legPlacementAngle * rotationMultiplier[i], Vector2(0, distanceFromCenter));


    // --- Part 2: Apply Body Orientation Compensation (Roll, Pitch, Yaw) ---

    // Calculate the body rotation matrix including roll, pitch, and yaw
    Vector3 C = {0, 0, distanceFromGround};  // Center of body
    Matrix3x3 R = makeRotationMatrix(radians(roll), radians(pitch), radians(yaw));

    // Get the unrotated hip position (relative to center, global frame)
    Vector3 unrotatedHipGlobal = legAttachmentOffsets[i];

    // Step 1: Calculate the actual rotated hip position in global coordinates
    Vector3 rotatedHipGlobal = C+ (R * unrotatedHipGlobal-C);

    // Step 2: Calculate the desired *absolute global* position for the foot
    // This is where the foot *would be* if the body moved according to joysticks but wasn't tilted/rotated.
    Vector3 desiredFootWorldTarget = convertLocalLegPointToGlobal(desiredLocalPos, i);

    // Step 3: Calculate the vector from the actual rotated hip to the desired global foot position
    Vector3 vectorHipToFootGlobal = desiredFootWorldTarget - rotatedHipGlobal;

    // Step 4: Rotate this global vector into the leg's final local coordinate system
    float theta = globalLegPlacementRadians[i]; // Angle of leg relative to Global +X (Right)
    float cosTheta = cos(theta);
    float sinTheta = sin(theta);

    // Project the global vector onto the local axes
    float finalTargetLocalX = vectorHipToFootGlobal.x * (-sinTheta) + vectorHipToFootGlobal.y * cosTheta;
    float finalTargetLocalY = vectorHipToFootGlobal.x * cosTheta + vectorHipToFootGlobal.y * sinTheta;
    float finalTargetLocalZ = vectorHipToFootGlobal.z;

    Vector3 finalTargetLocal = Vector3(finalTargetLocalX, finalTargetLocalY, finalTargetLocalZ);

    // --- Debugging ---
    float dist = sqrt(
      finalTargetLocal.x * finalTargetLocal.x +
      finalTargetLocal.y * finalTargetLocal.y +
      finalTargetLocal.z * finalTargetLocal.z
    );
    Serial.print("Leg "); Serial.println(i);
    print_value("Desired Local (Pre-Orient)", desiredLocalPos, true);
    print_value("Desired Global (Pre-Orient)", desiredFootWorldTarget, true);
    print_value("Final Target Local (Post-Orient)", finalTargetLocal, true);
    print_value("Vector Hip->Foot (Global)", vectorHipToFootGlobal, true);
    print_value("Rotated Hip World", rotatedHipGlobal, true);
    print_value("Unrotated Hip (Offset)", unrotatedHipGlobal, true);
    Serial.print("  Distance to foot: "); Serial.println(dist);


    // Step 5: Move leg to the final target position
    moveToPos(i, finalTargetLocal);
  }

  Serial.println("=====================");
}
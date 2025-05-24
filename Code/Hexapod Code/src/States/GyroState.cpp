#include <Arduino.h>
#include "State.h"
#include "LegManager.h"
#include "NRF.h"
#include <math.h> // Include for sin, cos, radians

// --- Helper Function Declarations (ensure these match your definitions) ---
Vector3 convertLocalLegPointToGlobal(Vector3 localLegPoint, int legIndex);
Vector3 convertGlobalLegPointToLocal(Vector3 globalLegPoint, int legIndex);
// --- End Helper Declarations ---

// --- Configuration ---
// Adjust sensitivity factors as needed
const float tiltSensitivity = 0.6f;
const float rotationSensitivity = 0.3f; // Sensitivity for turning with right joystick
// --- End Configuration ---


void GyroState::init()
{
  Serial.println("Gyro State.");
  for (int i = 0; i < 6; i++)
  {
    // Ensure basePoints[i] represents the neutral position in the *local leg frame*
    basePoints[i] = currentLegPositions[i]; // Assuming currentLegPositions are local neutral points
  }
  // Initialize smoothed vectors
  joy1CurrentVector = Vector2(0,0);
  joy2CurrentVector = Vector2(0,0);
  gyroCurrentVector = Vector2(0,0);
}

void GyroState::exit()
{
  Serial.println("Exiting Gyro State.");
}

void GyroState::loop()
{
  // --- Input Mapping & Smoothing ---
  double joy1x = map(rc_control_data.joyLeft_X, 0, 254, -100, 100); // Strafe Left/Right
  double joy1y = map(rc_control_data.joyLeft_Y, 0, 254, -100, 100); // Move Forward/Backward

  double joy2x = map(rc_control_data.joyRight_X, 0, 254, -100, 100); // Rotate Left/Right
  double joy2y = map(rc_control_data.joyRight_Y, 0, 254, -100, 100); // Unused

  double gyroX = constrain(rc_control_data.gyro_X, -45, 45); // Roll angle (degrees)
  double gyroY = constrain(rc_control_data.gyro_Y, -45, 45); // Pitch angle (degrees)

  joy1TargetVector = Vector2(joy1x, joy1y);
  joy2TargetVector = Vector2(joy2x, joy2y);

  joy1CurrentVector = lerp(joy1CurrentVector, joy1TargetVector, 0.02);
  joy2CurrentVector = lerp(joy2CurrentVector, joy2TargetVector, 0.03);

  gyroTargetVector = Vector2(gyroY, gyroX);
  gyroCurrentVector = lerp(gyroCurrentVector, gyroTargetVector, 0.06); // Smoothed roll (x), pitch (y)

  // --- Kinematics Calculation ---
  Vector3 localStrideTargetPoint;
  Vector3 bodyTargetPoint; // Target point in the body frame (origin at center)
  Vector3 finalLocalTargetPoint; // Final target point in the local leg frame

  // Convert smoothed gyro degrees to radians
  float pitch_rad = radians(gyroCurrentVector.y); // Y is pitch
  float roll_rad  = radians(-gyroCurrentVector.x); // X is roll

  // Calculate body rotation angle from right joystick
  float bodyRotationAngle_rad = radians(joy2CurrentVector.x * rotationSensitivity);

  for (int i = 0; i < 6; i++)
  {
    // 1. Calculate Target with Stride in Local Leg Frame
    localStrideTargetPoint = basePoints[i]; // Start with neutral local position
    // Apply stride translation locally (adjust axes based on your local frame definition)
    localStrideTargetPoint.x += joy1CurrentVector.y * strideMultiplier[i]; // Left stick Y -> local X stride
    localStrideTargetPoint.y += -joy1CurrentVector.x * strideMultiplier[i]; // Left stick X -> local Y stride
    localStrideTargetPoint = localStrideTargetPoint.rotate(legPlacementAngle * rotationMultiplier[i], Vector2(0, distanceFromCenter)); // Rotate to leg frame

    // 2. Convert Local Stride Target to Global/Body Frame
    bodyTargetPoint = convertLocalLegPointToGlobal(localStrideTargetPoint, i);

    // 3. Apply Body Rotation (around body center Z-axis) based on right joystick
    float currentBodyX_preRotate = bodyTargetPoint.x;
    float currentBodyY_preRotate = bodyTargetPoint.y;
    bodyTargetPoint.x = currentBodyX_preRotate * cos(bodyRotationAngle_rad) - currentBodyY_preRotate * sin(bodyRotationAngle_rad);
    bodyTargetPoint.y = currentBodyX_preRotate * sin(bodyRotationAngle_rad) + currentBodyY_preRotate * cos(bodyRotationAngle_rad);

    // 4. Apply Body Tilt (Pitch and Roll) - Adjust Z based on body frame position
    float currentBodyX_postRotate = bodyTargetPoint.x;
    float currentBodyY_postRotate = bodyTargetPoint.y;
    float deltaZ_pitch = -currentBodyX_postRotate * sin(pitch_rad);
    float deltaZ_roll = -currentBodyY_postRotate * sin(roll_rad); // Adjust sign based on Y-axis direction
    bodyTargetPoint.z += (deltaZ_pitch + deltaZ_roll) * tiltSensitivity;

    // 5. Convert Final Body Target Point back to Local Leg Frame
    finalLocalTargetPoint = convertGlobalLegPointToLocal(bodyTargetPoint, i);

    // 6. Move Leg to Final Local Frame Position
    // Assuming moveToPos(legIndex, targetPositionInLocalLegFrame)
    moveToPos(i, finalLocalTargetPoint);

    // Optional: Print gyro values once per loop outside the for loop if needed
    // if (i == 0) print_value("Gyro", gyroCurrentVector, true);
  }
  // Example: Print gyro values once after the loop
  // print_value("Gyro", gyroCurrentVector, true);
}
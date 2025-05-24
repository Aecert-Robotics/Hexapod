#include "LegManager.h"
#include "NRF.h"
#include <EEPROM.h>

Vector3 currentLegPositions[NUM_LEGS];
// Vector3 cycleStartPositions[NUM_LEGS];

PWMServo coxaServos[NUM_LEGS];
PWMServo femurServos[NUM_LEGS];
PWMServo tibiaServos[NUM_LEGS];

int8_t calibrationOffsets[18] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

Vector3 posToAngle(Vector3 pos)
{
    float dis = Vector3(0, 0, 0).distanceTo(pos);
    if (dis > totalLegLength)
    {
        Serial.println("Position out of reach");
        return Vector3(0, 0, 0);
    }

    float x = pos.x;
    float y = pos.y;
    float z = pos.z;

    float theta1 = atan2(y, x) * (180 / PI); // base angle
    float l = sqrt(x * x + y * y);
    float l1 = l - coxaLength;
    float h = sqrt(l1 * l1 + z * z);

    float phi1 = acos(constrain((pow(h, 2) + pow(femurLength, 2) - pow(tibiaLength, 2)) / (2 * h * femurLength), -1, 1));
    float phi2 = atan2(z, l1);
    float theta2 = (phi1 + phi2) * 180 / PI;
    float phi3 = acos(constrain((pow(femurLength, 2) + pow(tibiaLength, 2) - pow(h, 2)) / (2 * femurLength * tibiaLength), -1, 1));
    float theta3 = (phi3 * 180 / PI);

    return Vector3(
        180 - theta1 + COXA_BASE_OFFSET, // tibia
        theta2 + FEMUR_BASE_OFFSET,      // femur
        180 - theta3 + TIBIA_BASE_OFFSET // coxa
    );
}

void setLegAngles(int leg, Vector3 angles)
{
    coxaServos[leg].write(angles.x + calibrationOffsets[leg * 3]);
    femurServos[leg].write(angles.y + calibrationOffsets[leg * 3 + 1]);
    tibiaServos[leg].write(angles.z + calibrationOffsets[leg * 3 + 2]);
}

void moveToPos(int leg, Vector3 pos)
{
    hex_sensor_data.xPositions[leg] = (int)pos.x;
    hex_sensor_data.yPositions[leg] = (int)pos.y;
    currentLegPositions[leg] = pos;

    Vector3 angles = posToAngle(pos);
    setLegAngles(leg, angles);
}

void attachServos()
{
    for (int i = 0; i < NUM_LEGS; i++)
    {
        coxaServos[i].attach(COXA_PINS[i]);
        femurServos[i].attach(FEMUR_PINS[i]);
        tibiaServos[i].attach(TIBIA_PINS[i]);
    }
}

void saveCalibrationOffsets()
{
    for (int i = 0; i < 18; i++)
    {
        EEPROM.put(i * sizeof(int8_t), calibrationOffsets[i]);
    }
    Serial.println("Calibration Offsets saved to EEPROM.");
}

void loadCalibrationOffsets()
{
    Serial.println("Calibration Offsets loaded from EEPROM.");
    for (int i = 0; i < 18; i++)
    {
        int8_t val;
        EEPROM.get(i * sizeof(int8_t), val);
        calibrationOffsets[i] = val;
    }
}

Vector3 convertLocalLegPointToGlobal(Vector3 localLegPoint, int legIndex)
{
    // Assumes globalLegPlacementRadians are angles CCW from Global +X (Right)
    // Assumes Local Frame: +x Left (relative to leg), +y Outward (along leg)
    // Assumes Global Frame: +X Right, +Y Forward

    float distanceFromCenterToLegBase = 110;
    if (legIndex == 1 || legIndex == 4)
    {
        distanceFromCenterToLegBase = 95;
    }
    // Angle relative to Global +X (Right)
    float theta = globalLegPlacementRadians[legIndex];
    float cosTheta = cos(theta);
    float sinTheta = sin(theta);

    // 1. Calculate base position using standard polar to Cartesian
    float baseX = distanceFromCenterToLegBase * cosTheta;
    float baseY = distanceFromCenterToLegBase * sinTheta;

    // 2. Transform local point to global frame offset
    // Global offset = localX * (Unit vector for local +x) + localY * (Unit vector for local +y)
    // Unit vector for local +y (Outward) is (cosTheta, sinTheta)
    // Unit vector for local +x (Left) is (-sinTheta, cosTheta)
    float rotatedRelX = localLegPoint.x * (-sinTheta) + localLegPoint.y * cosTheta;
    float rotatedRelY = localLegPoint.x * cosTheta + localLegPoint.y * sinTheta;

    // 3. Add base position to the rotated local offset
    float globalX = baseX + rotatedRelX;
    float globalY = baseY + rotatedRelY;
    float globalZ = localLegPoint.z; // Z remains the same

    return Vector3(globalX, globalY, globalZ);
}

Vector3 convertGlobalLegPointToLocal(Vector3 globalPoint, int legIndex)
{
    // Assumes globalLegPlacementRadians are angles CCW from Global +X (Right)
    // Assumes Local Frame: +x Left (relative to leg), +y Outward (along leg)
    // Assumes Global Frame: +X Right, +Y Forward

    float distanceFromCenterToLegBase = 110;
    if (legIndex == 1 || legIndex == 4)
    {
        distanceFromCenterToLegBase = 95;
    }
    // Angle relative to Global +X (Right)
    float theta = globalLegPlacementRadians[legIndex];
    float cosTheta = cos(theta);
    float sinTheta = sin(theta);

    // 1. Calculate base position
    float baseX = distanceFromCenterToLegBase * cosTheta;
    float baseY = distanceFromCenterToLegBase * sinTheta;

    // 2. Calculate vector from base to global point in global frame
    float relX_global = globalPoint.x - baseX;
    float relY_global = globalPoint.y - baseY;

    // 3. Project the relative global vector onto the local axes
    // localX = Dot product of rel_global with local +x unit vector (-sin, cos)
    // localY = Dot product of rel_global with local +y unit vector (cos, sin)
    float localX = relX_global * (-sinTheta) + relY_global * cosTheta;
    float localY = relX_global * cosTheta + relY_global * sinTheta;
    float localZ = globalPoint.z; // Z remains the same

    return Vector3(localX, localY, localZ);
}
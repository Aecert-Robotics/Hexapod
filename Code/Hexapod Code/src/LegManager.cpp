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
    Serial.println("calibrationOffsets saved to EEPROM.");
    printCalibrationOffsets();
}

void loadCalibrationOffsets()
{
    Serial.println("calibrationOffsets loaded from EEPROM.");
    for (int i = 0; i < 18; i++)
    {
        int8_t val;
        EEPROM.get(i * sizeof(int8_t), val);
        calibrationOffsets[i] = val;
    }
    printCalibrationOffsets();
}

void printCalibrationOffsets()
{
  
  Serial.print("Raw Offsets: ");
  for (int i = 0; i < 18; i++)
  {

    Serial.print(calibrationOffsets[i]);
    if (i < 17)
    {
      Serial.print(" ");
    }
  }
  Serial.println();

  Serial.print("EEPROM: ");
  for (int i = 0; i < 18; i++)
  {
    int8_t val;
    EEPROM.get(i * sizeof(int8_t), val);
    Serial.print(val);
    if (i < 17)
    {
      Serial.print(" ");
    }
  }
  Serial.println();
}
#pragma once

#include <Arduino.h>
#include "RF24.h"
#include "utils.h"

extern RF24 radio;

enum PackageType
{
    RC_CONTROL_DATA = 1,
    RC_SETTINGS_DATA = 2,
    HEXAPOD_SETTINGS_DATA = 3,
    HEXAPOD_SENSOR_DATA = 4
};

// 10 Bytes Total
struct RC_Control_Data_Package
{
    byte type; // 1 byte

    byte joyLeft_X; // 1 byte
    byte joyLeft_Y; // 1 byte

    byte joyRight_X; // 1 byte
    byte joyRight_Y; // 1 byte

    byte potLeft;  // 1 byte
    byte potRight; // 1 byte

    int8_t gyro_X; // 1 byte
    int8_t gyro_Y; // 1 byte

    // 1 byte
    byte button_A : 1; // 1 bit
    byte button_B : 1; // 1 bit
    byte button_C : 1; // 1 bit
    byte button_D : 1; // 1 bit
    byte toggle_A : 1; // 1 bit
    byte toggle_B : 1; // 1 bit
    byte toggle_C : 1; // 1 bit
    byte toggle_D : 1; // 1 bit

    // 1 byte
    byte bumper_A : 1;        // 1 bit
    byte bumper_B : 1;        // 1 bit
    byte bumper_C : 1;        // 1 bit
    byte bumper_D : 1;        // 1 bit
    byte joyLeft_Button : 1;  // 1 bit
    byte joyRight_Button : 1; // 1 bit
    byte reserved : 2;        // 2 bits padding

    byte gait; // 1 byte
};

// 20 Bytes Total
struct RC_Settings_Data_Package
{
    byte type; // 1 byte

    byte calibrating : 1;   // 1 bit
    byte increaseValue : 1; // 1 bit
    byte decreaseValue : 1; // 1 bit
    byte reserved : 5;      // 7 bits padding, 1 byte total

    int8_t calibrationIndex; // 1 byte
};

// 19 Bytes Total
struct Hexapod_Settings_Data_Package
{
    byte type;                     // 1 byte
    int8_t calibrationOffsets[18]; // 18 bytes
};

// 29 Bytes Total
struct Hexapod_Sensor_Data_Package
{
    byte type;            // 1 byte
    int8_t xPositions[6]; // 6 bytes
    int8_t yPositions[6]; // 6 bytes
};

// Declare the data package variables
extern RC_Control_Data_Package rc_control_data;
extern RC_Settings_Data_Package rc_settings_data;
extern Hexapod_Settings_Data_Package hex_settings_data;
extern Hexapod_Sensor_Data_Package hex_sensor_data;

extern byte receiveType;
extern byte sendType;

void setupNRF();
bool receiveNRFData();
void RC_DisplayData();
void initializeControllerPayload();
void initializeHexPayload();
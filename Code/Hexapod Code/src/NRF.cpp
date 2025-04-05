#include <Arduino.h>
#include <NRF.h>

uint8_t address[6] = "HEX01";

// Declare the data package variables
RC_Control_Data_Package rc_control_data;
RC_Settings_Data_Package rc_settings_data;
Hexapod_Settings_Data_Package hex_settings_data;
Hexapod_Sensor_Data_Package hex_sensor_data;

#define CE_PIN 26
#define CSN_PIN 27
#define INTERVAL_MS_SIGNAL_LOST 1000
#define INTERVAL_MS_SIGNAL_RETRY 250
unsigned long rc_last_received_time = 0;

RF24 radio(CE_PIN, CSN_PIN);

unsigned long lastSignalMillis = 0;
void setupNRF()
{
    if (!radio.begin())
    {
        Serial.println(F("radio hardware is not responding!!"));
        while (1)
        {
        } // hold in infinite loop
    }
    else
    {
        Serial.println(F("radio hardware is ready!"));
    }

    radio.setAddressWidth(5);
    radio.setPALevel(RF24_PA_LOW);
    radio.setPayloadSize(32); // Set the payload size to the maximum of 32 bytes
    radio.setChannel(124);
    radio.openReadingPipe(1, address);
    radio.enableAckPayload();
    radio.startListening();

    initializeHexPayload();
    initializeControllerPayload();

    radio.writeAckPayload(1, &hex_sensor_data, sizeof(hex_sensor_data));
}

void initializeHexPayload()
{
    hex_sensor_data.type = HEXAPOD_SENSOR_DATA;

    hex_settings_data.type = HEXAPOD_SETTINGS_DATA;
    Serial.println("Filling hex_settings_data.calibrationOffsets with 0's.");
    for (int i = 0; i < 18; i++)
    {
        hex_settings_data.calibrationOffsets[i] = 0;
    }
}

void initializeControllerPayload()
{

    // control package
    rc_control_data.type = RC_CONTROL_DATA;

    rc_control_data.joyLeft_X = 127;
    rc_control_data.joyLeft_Y = 127;
    rc_control_data.joyRight_X = 127;
    rc_control_data.joyRight_Y = 127;
    rc_control_data.potLeft = 50;
    rc_control_data.potRight = 50;
    rc_control_data.button_A = UNPRESSED;
    rc_control_data.button_B = UNPRESSED;
    rc_control_data.button_C = UNPRESSED;
    rc_control_data.button_D = UNPRESSED;
    rc_control_data.toggle_A = OFF;
    rc_control_data.toggle_B = OFF;
    rc_control_data.toggle_C = OFF;
    rc_control_data.toggle_D = OFF;
    rc_control_data.bumper_A = UNPRESSED;
    rc_control_data.bumper_B = UNPRESSED;
    rc_control_data.bumper_C = UNPRESSED;
    rc_control_data.bumper_D = UNPRESSED;
    rc_control_data.joyLeft_Button = UNPRESSED;
    rc_control_data.joyRight_Button = UNPRESSED;
    rc_control_data.gait = 0;

    // settings package
    rc_settings_data.type = RC_SETTINGS_DATA;
    rc_settings_data.calibrating = 0;
    rc_settings_data.increaseValue = UNPRESSED;
    rc_settings_data.decreaseValue = UNPRESSED;
    rc_settings_data.calibrationIndex = -1; // -1 means no calibration index is set
}

byte receiveType = RC_CONTROL_DATA;

bool receiveNRFData()
{
    // This device is a RX node
    uint8_t pipe;
    if (radio.available(&pipe))
    {
        byte incomingType;
        radio.read(&incomingType, sizeof(incomingType));
        if (receiveType != incomingType && incomingType != 0){
            receiveType = incomingType;
            if (receiveType == RC_CONTROL_DATA)
            {
                Serial.println("Receiving CONTROL");
                Serial.println("Sending SENSOR");
            }
            else if (receiveType == RC_SETTINGS_DATA)
            {
                Serial.println("Receiving SETTINGS");
                Serial.println("Sending SETTINGS");
            }
        }
            

        if (receiveType == RC_CONTROL_DATA)
        {
            radio.read(&rc_control_data, sizeof(rc_control_data)); 
            radio.writeAckPayload(1, &hex_sensor_data, sizeof(hex_sensor_data)); // load the payload for the next time
            //print the hex sensor data foot positions
            
            /*
            for (int i = 0; i < 6; i++)
            {
                Serial.print("Foot position ");
                Serial.print(i);
                Serial.print(": ");
                Serial.print(hex_sensor_data.xPositions[i]);
                Serial.print(", ");
                Serial.println(hex_sensor_data.yPositions[i]);
            }
            */
            
            
        }

        else if (receiveType == RC_SETTINGS_DATA)
        {
            radio.read(&rc_settings_data, sizeof(rc_settings_data));
            radio.writeAckPayload(1, &hex_settings_data, sizeof(hex_settings_data)); // load the payload for the next time
        }

        rc_last_received_time = millis();
    }

    if (millis() - rc_last_received_time > INTERVAL_MS_SIGNAL_LOST)
        return false;

    return true;
}
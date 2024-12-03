#include "arduino_secrets.h"

#include <ArduinoLowPower.h>
#include <MKRWAN.h>

// Constants for the TDS sensor
const int TDS_SENSOR_PIN = A0;          // Analog pin connected to the TDS sensor
const float voltageReference = 3.3;     // MKR WAN 1310 operates at 3.3V
const int calibrationFactor = 133;      // Calibration factor for TDS sensor

// LoRaWAN settings
LoRaModem modem;  // Create an instance of LoRaModem

// Variables for TDS reading
int rawValue = 0;          // Raw value from the TDS sensor
float voltage = 0.0;       // Voltage read from the sensor
float TDS = 0.0;           // Total Dissolved Solids (TDS) in parts per million (PPM)

void setup() {
  // Initialize Serial Monitor for debugging
  Serial.begin(9600);
  while (!Serial); // Wait for Serial to be ready
  Serial.println("Initializing LoRaWAN connection...");

  // Initialize the LoRaWAN modem
  if (!modem.begin(US915)) {  
    Serial.println("Failed to initialize LoRaWAN modem!");
    while (1);  // Halt if LoRaWAN failed
  }

  Serial.println("Now Disabling all channels and enable channel 1 only for Helium ");

  modem.disableChannel(0);
  modem.enableChannel(1);    // only one enabled for Helium
  modem.disableChannel(2);
  modem.disableChannel(3);
  modem.disableChannel(4);
  modem.disableChannel(5);
  modem.disableChannel(6);
  delay(2000);
  Serial.println("Now Joining the Helium Network ");

  Serial.println("LoRaWAN modem initialized successfully.");

  // Attempt to join the LoRaWAN network using OTAA
  Serial.println("Attempting to join the LoRaWAN network via OTAA...");
  int joinAttempts = 0;
  const int maxJoinAttempts = 20;
  
  while (!modem.joinOTAA(appEui, appKey)) {
    Serial.println("Join attempt failed. Retrying in 5 seconds...");
    delay(5000); // Wait 5 seconds before retrying
    joinAttempts++;
    if (joinAttempts >= maxJoinAttempts) {
      Serial.println("Max join attempts reached. Check your credentials and try again.");
      while (1);  // Halt if join failed
    }
  }

  Serial.println("Successfully joined the LoRaWAN network!");
}

unsigned long startTime;
unsigned long endTime;

void loop() {
  // Get routine's start time
  startTime = millis();

  // Read the raw value from the TDS sensor
  rawValue = analogRead(TDS_SENSOR_PIN);

  // Convert the raw value to voltage
  voltage = (rawValue * voltageReference) / 1024.0;

  // Calculate the TDS value (PPM) based on raw sensor data
  TDS = rawValue * calibrationFactor / 1024.0;

  // Print the TDS values to the Serial Monitor (for debugging)
  Serial.print("Raw Sensor Value: ");
  Serial.print(rawValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage, 3);
  Serial.print(" V | TDS (PPM): ");
  Serial.println(TDS);

  // Prepare the payload to send
  String payload = "TDS:" + String(TDS, 2);  // e.g., "TDS:123.45"

  // Send data via LoRaWAN
  Serial.println("Sending data via LoRaWAN...");
  do {
    modem.beginPacket();            // Begin the packet transmission
    modem.print(payload);           // Write the payload
  } while (!modem.endPacket());     // Retry until succeed

  // Get routine's finish time
  endTime = millis();

  // Delay to prevent flooding the network with data
  LowPower.sleep(600000 - (endTime - startTime)) // Send data every 10 minutes
}

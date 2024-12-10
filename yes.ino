#include "arduino_secrets.h"
#include "nutri.h"

#include <ArduinoLowPower.h>
#include <MKRWAN.h>

// LoRaWAN settings
LoRaModem modem;  // Create an instance of LoRaModem

// Nutrient sensor
float payload = 0.0;

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

  // Obtain nutrient readings
  payload = getNutri();

  // Send data via LoRaWAN
  Serial.println("Sending data via LoRaWAN...");
  modem.beginPacket();     // Begin the packet transmission
  modem.print(payload);    // Write the payload
  modem.endPacket();       // Retry until succeed

  // Get routine's finish time
  endTime = millis();

  // Delay to prevent flooding the network with data
  LowPower.sleep(600000 - (endTime - startTime)); // Send data every 10 minutes
}

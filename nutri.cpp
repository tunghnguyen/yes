#include "nutri.h"

#include <MKRWAN.h>

// Constants for the TDS sensor
const int TDS_SENSOR_PIN = A0;
const float voltageReference = 5.0;     // The analog reference voltage is 5.0V
const int calibrationFactor = 133.42;      // Calibration factor for TDS sensor

// Variables for TDS reading
int rawValue = 0;          // Raw value from the TDS sensor
float voltage = 0.0;       // Voltage read from the sensor
float TDS = 0.0;           // Total Dissolved Solids (TDS) in parts per million (PPM)

float getNutri() {
  // Read the raw value from the TDS sensor
  rawValue = analogRead(TDS_SENSOR_PIN);

  // Convert the raw value to voltage
  voltage = (rawValue * voltageReference) / 1024.0;

  // Calculate the TDS value (PPM) based on raw sensor data
  TDS = rawValue * calibrationFactor;

  // Print the TDS values to the Serial Monitor (for debugging)
  Serial.print("Raw Sensor Value: ");
  Serial.print(rawValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage, 3);
  Serial.print(" V | TDS (PPM): ");
  Serial.println(TDS);

  return TDS;
}

// Takes heartrate and SpO2 measurements from MAX30101/32664
// Prints to Serial Monitor

#include <Wire.h>
#include <SparkFun_Bio_Sensor_Hub_Library.h>

constexpr uint8_t resetPin = 10;
constexpr uint8_t mfioPin = 9;

SparkFun_Bio_Sensor_Hub bioHub(resetPin, mfioPin);

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (bioHub.begin() != 0) {
    Serial.println(F("Sensor not found."));
    while (true) {
      delay(1000);
    }
  }

  Serial.println(F("Sensor connected."));

  // Include SpO2 meatrics
  int error = bioHub.configBpm(MODE_TWO);

  if (error != 0) {
    Serial.print(F("Sensor configuration failed. Error: "));
    Serial.println(error);

    while (true) {
      delay(1000);
    }
  }

  Serial.println(F("Sensor configured."));
  Serial.println(F("Place finger on sensor."));

  delay(4000);
}

void loop() {
  bioData reading = bioHub.readBpm();

  Serial.print(F("Heart rate: "));
  Serial.println(reading.heartRate);

  Serial.print(F("Confidence: "));
  Serial.println(reading.confidence);

  Serial.print(F("Oxygen: "));
  Serial.println(reading.oxygen);

  Serial.print(F("Status: "));
  Serial.println(reading.status);

  Serial.print(F("Extended status: "));
  Serial.println(reading.extStatus);

  Serial.print(F("R value: "));
  Serial.println(reading.rValue);

  Serial.println();

  delay(2000);
}

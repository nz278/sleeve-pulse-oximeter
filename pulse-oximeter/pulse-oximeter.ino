// Integrated prototype system demonstration
// Heart rate and SpO2 measurements are sent to the Bluefruit Connect UART terminal
// Haptic motor provides a warning when several readings fall below threshold

#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_Bio_Sensor_Hub_Library.h>
#include "Adafruit_BluefruitLE_UART.h"

constexpr uint8_t modePin = 12;

constexpr uint8_t resetPin = 10;
constexpr uint8_t mfioPin = 9;

constexpr uint8_t motorPin = 6;

// Alert settings
constexpr int lowHeartRateThreshold = 50;
constexpr int lowOxygenThreshold = 92;
constexpr int minimumConfidence = 80;
constexpr uint8_t readingsBeforeAlert = 3;

SparkFun_Bio_Sensor_Hub bioHub(resetPin, mfioPin);
Adafruit_BluefruitLE_UART ble(Serial1, modePin);

uint8_t lowReadingCount = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  pinMode(motorPin, OUTPUT);
  digitalWrite(motorPin, LOW);

  delay(500);

  if (!ble.begin(false)) {
    fail(F("Bluefruit not found."));
  }

  ble.echo(false);
  ble.verbose(false);

  Serial.println(F("Waiting for Bluetooth connection."));

  while (!ble.isConnected()) {
    delay(500);
  }

  Serial.println(F("Bluefruit connected."));
  sendMessage(F("Bluefruit connected."));

  if (bioHub.begin() != 0) {
    fail(F("Sensor not found."));
  }

  sendMessage(F("Sensor connected."));

  int error = bioHub.configBpm(MODE_TWO);

  if (error != 0) {
    fail(F("Sensor configuration failed."));
  }

  sendMessage(F("Sensor configured."));
  sendMessage(F("Place finger on sensor."));

  delay(4000);
}

void loop() {
  bioData reading = bioHub.readBpm();

  // Ignore unreliable readings (e.g. poor contact)
  bool readingIsReliable =
      reading.status == 3 &&
      reading.confidence >= minimumConfidence;

  if (!readingIsReliable) {
    lowReadingCount = 0;
    digitalWrite(motorPin, LOW);

    sendMessage(F("Realign finger or wrist."));
    delay(1000);
    return;
  }

  // Send to Bluefruit Connect app
  sendMeasurement(F("Heart rate: "), reading.heartRate);
  sendMeasurement(F("Confidence: "), reading.confidence);
  sendMeasurement(F("Oxygen: "), reading.oxygen);

  bool lowHeartRate =
      reading.heartRate < lowHeartRateThreshold;

  bool lowOxygen =
      reading.oxygen <= lowOxygenThreshold;

  // Alert logic
  if (lowHeartRate || lowOxygen) {
    if (lowReadingCount < readingsBeforeAlert) {
      ++lowReadingCount;
    }
  } else {
    lowReadingCount = 0;
    digitalWrite(motorPin, LOW);
  }

  if (lowReadingCount >= readingsBeforeAlert) {
    sendAlert(lowHeartRate, lowOxygen);
    buzzMotor();
  }

  delay(1000);
}

// Alert mechanism
void sendAlert(bool lowHeartRate, bool lowOxygen) {
  if (lowHeartRate && lowOxygen) {
    sendMessage(F("Warning: heart rate and oxygen are low."));
  } else if (lowHeartRate) {
    sendMessage(F("Warning: heart rate is low."));
  } else {
    sendMessage(F("Warning: oxygen is low."));
  }
}

// Short haptic warning pulse
void buzzMotor() {
  digitalWrite(motorPin, HIGH);
  delay(300);
  digitalWrite(motorPin, LOW);
}

// Text message
void sendMessage(const __FlashStringHelper* message) {
  ble.print(F("AT+BLEUARTTX="));
  ble.println(message);
  ble.waitForOK();
}

// Numerical measurements
void sendMeasurement(
    const __FlashStringHelper* label,
    int value
) {
  ble.print(F("AT+BLEUARTTX="));
  ble.print(label);
  ble.println(value);
  ble.waitForOK();
}

void fail(const __FlashStringHelper* message) {
  Serial.println(message);
  digitalWrite(motorPin, LOW);

  while (true) {
    delay(1000);
  }
}

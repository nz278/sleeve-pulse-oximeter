// Simple test of haptic motor
// Buzzes for a second intermittently

#include <Arduino.h>

const uint8_t motorPin = 10;
const unsigned long pulseDurationMs = 1000;

void setup() {
  pinMode(motorPin, OUTPUT);
  digitalWrite(motorPin, LOW);
}

void loop() {
  digitalWrite(motorPin, HIGH);
  delay(pulseDurationMs);

  digitalWrite(motorPin, LOW);
  delay(pulseDurationMs);
}
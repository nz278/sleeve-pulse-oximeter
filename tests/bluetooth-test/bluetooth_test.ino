// Tests UART communication with Adafruit Bluefruit BLE module
// Adapted from example code in documentation
// Transmits text entered from Arduino Serial Monitor to Bluefruit Connect app

#include <Arduino.h>
#include "Adafruit_BluefruitLE_UART.h"

constexpr uint8_t modePin = 12;
constexpr size_t bufferSize = 128;

Adafruit_BluefruitLE_UART ble(Serial1, modePin);

void setup() {
  Serial.begin(115200);
  delay(500);

  if (!ble.begin(false)) {
    fail(F("Bluefruit not found."));
  }

  // Could use for debugging
  ble.echo(false);
  ble.verbose(false);

  while (!ble.isConnected()) {
    delay(500);
  }

  Serial.println(F("Bluefruit connected."));
}

void loop() {
  sendData();
  receiveData();
}

void sendData() {
  if (!Serial.available()) {
    return;
  }

  char buffer[bufferSize + 1];

  size_t length = Serial.readBytesUntil('\n', buffer, bufferSize);

  if (length > 0 && buffer[length - 1] == '\r') {
    --length;
  }

  buffer[length] = '\0';

  if (length == 0) {
    return;
  }

  ble.print(F("AT+BLEUARTTX="));
  ble.println(buffer);

  ble.waitForOK();
}

void receiveData() {
  ble.println(F("AT+BLEUARTRX"));

  if (!ble.readline()) {
    return;
  }

  if (strcmp(ble.buffer, "OK") == 0) {
    return;
  }

  Serial.println(ble.buffer);

  // Trailing "OK"
  ble.waitForOK();
}

void fail(const __FlashStringHelper* message) {
  Serial.println(message);

  while (true) {
    delay(1000);
  }
}

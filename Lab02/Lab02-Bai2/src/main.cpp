#include <Arduino.h>

const int BUTTON = 18;
const int LED1 = 4;
const int LED2 = 5;

bool ledState = LOW;
bool buttonState = HIGH;
bool lastButtonReading = HIGH;

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  digitalWrite(LED1, ledState);
  digitalWrite(LED2, ledState);
}

void loop() {
  bool reading = digitalRead(BUTTON);

  if (reading != lastButtonReading) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        ledState = !ledState;
        digitalWrite(LED1, ledState);
        digitalWrite(LED2, ledState);
      }
    }
  }

  lastButtonReading = reading;
}
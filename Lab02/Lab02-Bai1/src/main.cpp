#include <Arduino.h>

const int LED_1 = 2;
const int LED_2 = 4;

const unsigned long intervalLED1 = 500;
const unsigned long intervalLED2 = 1200;

unsigned long previousMillisLED1 = 0;
unsigned long previousMillisLED2 = 0;

bool stateLED1 = LOW;
bool stateLED2 = LOW;

void setup() {
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillisLED1 >= intervalLED1) {
    previousMillisLED1 = currentMillis;
    stateLED1 = !stateLED1;
    digitalWrite(LED_1, stateLED1);
  }

  if (currentMillis - previousMillisLED2 >= intervalLED2) {
    previousMillisLED2 = currentMillis;
    stateLED2 = !stateLED2;
    digitalWrite(LED_2, stateLED2);
  }
}
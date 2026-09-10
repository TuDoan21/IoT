#include <Arduino.h>

#define LED_RED     23
#define LED_YELLOW  22
#define LED_GREEN   21

enum TrafficLight {
  RED,
  GREEN,
  YELLOW
};

TrafficLight state = RED;
unsigned long previousMillis = 0;

void setLight(TrafficLight s) {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, LOW);

  switch (s) {
    case RED:
      digitalWrite(LED_RED, HIGH);
      Serial.println("Dang chay: DEN DO (5 giay)");
      break;

    case GREEN:
      digitalWrite(LED_GREEN, HIGH);
      Serial.println("Dang chay: DEN XANH (4 giay)");
      break;

    case YELLOW:
      digitalWrite(LED_YELLOW, HIGH);
      Serial.println("Dang chay: DEN VANG (2 giay)");
      break;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  setLight(RED);
  previousMillis = millis();
}

void loop() {
  unsigned long currentMillis = millis();

  switch (state) {
    case RED:
      if (currentMillis - previousMillis >= 5000) {
        state = GREEN;
        setLight(state);
        previousMillis = currentMillis;
      }
      break;

    case GREEN:
      if (currentMillis - previousMillis >= 4000) {
        state = YELLOW;
        setLight(state);
        previousMillis = currentMillis;
      }
      break;

    case YELLOW:
      if (currentMillis - previousMillis >= 2000) {
        state = RED;
        setLight(state);
        previousMillis = currentMillis;
      }
      break;
  }
}
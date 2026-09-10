#include <Arduino.h>

// Khai báo chân
const int LED1 = 4;
const int LED2 = 5;
const int BUTTON = 18;

// ===== Tác vụ 1: LED1 nhấp nháy =====
bool led1State = LOW;
unsigned long previousMillisLED = 0;
const unsigned long ledInterval = 300;

// ===== Tác vụ 2: Nút nhấn điều khiển LED2 =====
bool led2State = LOW;
bool buttonState = HIGH;
bool lastButtonReading = HIGH;

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// ===== Tác vụ 3: Đếm số vòng lặp trong 1 giây =====
unsigned long previousMillisCounter = 0;
unsigned long loopCount = 0;

void setup() {
  Serial.begin(115200);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);

  Serial.println("ESP32 - Bai 5: 3 Tac Vu Song Song");
}

void loop() {
  unsigned long currentMillis = millis();

  // Đếm số lần loop() chạy
  loopCount++;

  // ==========================
  // Tác vụ 1: LED1 nhấp nháy
  // ==========================
  if (currentMillis - previousMillisLED >= ledInterval) {
    previousMillisLED = currentMillis;
    led1State = !led1State;
    digitalWrite(LED1, led1State);
  }

  // ==========================
  // Tác vụ 2: Đọc nút nhấn
  // ==========================
  bool reading = digitalRead(BUTTON);

  if (reading != lastButtonReading) {
    lastDebounceTime = currentMillis;
  }

  if (currentMillis - lastDebounceTime >= debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        led2State = !led2State;
        digitalWrite(LED2, led2State);
      }
    }
  }

  lastButtonReading = reading;

  // ==========================
  // Tác vụ 3: Báo số vòng loop
  // ==========================
  if (currentMillis - previousMillisCounter >= 1000) {
    previousMillisCounter = currentMillis;

    Serial.print("So vong loop trong 1 giay: ");
    Serial.println(loopCount);

    loopCount = 0;
  }
}
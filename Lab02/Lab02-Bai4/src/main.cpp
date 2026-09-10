#include <Arduino.h>
#include <DHT.h>

// DHT22
#define DHTPIN 23
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Chân nút nhấn và LED
const int BUTTON = 18;
const int LED1 = 4;
const int LED2 = 5;

// Biến debounce
bool ledState = LOW;
bool buttonState = HIGH;
bool lastButtonReading = HIGH;

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// Biến đọc DHT22 định kỳ
unsigned long previousMillisDHT = 0;
const unsigned long dhtInterval = 2000;

void setup() {
  Serial.begin(115200);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);

  dht.begin();

  Serial.println("ESP32 - Bai 4: DHT22 + Button (Non-blocking)");
}

void loop() {
  unsigned long currentMillis = millis();

  // ===== Quét nút nhấn liên tục =====
  bool reading = digitalRead(BUTTON);

  if (reading != lastButtonReading) {
    lastDebounceTime = currentMillis;
  }

  if (currentMillis - lastDebounceTime >= debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      // INPUT_PULLUP: nhấn = LOW
      if (buttonState == LOW) {
        ledState = !ledState;
        digitalWrite(LED1, ledState);
        digitalWrite(LED2, ledState);
      }
    }
  }

  lastButtonReading = reading;

  // ===== Đọc DHT22 mỗi 2 giây =====
  if (currentMillis - previousMillisDHT >= dhtInterval) {
    previousMillisDHT = currentMillis;

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Loi doc DHT22!");
    } else {
      Serial.print("Nhiet do: ");
      Serial.print(temperature, 1);
      Serial.print(" °C | Do am: ");
      Serial.print(humidity, 1);
      Serial.println(" %");
    }
  }
}
#include <Arduino.h>

// LED tích hợp và LED ngoài cùng dùng GPIO 2
const int LED_PIN = 2;

// Chu kỳ thời gian (ms)
const unsigned long LED_TOGGLE_INTERVAL = 500;   // Đổi trạng thái mỗi 500ms
const unsigned long SERIAL_INTERVAL = 3000;      // Gửi Serial mỗi 3 giây

// Mốc thời gian riêng cho từng tiến trình
unsigned long previousMillisLED = 0;
unsigned long previousMillisSerial = 0;

// Trạng thái LED
bool ledState = LOW;

// Tiến trình 1: Heartbeat LED
void taskHeartbeatLED(unsigned long currentMillis) {
  if (currentMillis - previousMillisLED >= LED_TOGGLE_INTERVAL) {
    previousMillisLED = currentMillis;

    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }
}

// Tiến trình 2: Gửi báo cáo Serial
void taskSerialReport(unsigned long currentMillis) {
  if (currentMillis - previousMillisSerial >= SERIAL_INTERVAL) {
    previousMillisSerial = currentMillis;

    Serial.print("[System Heartbeat] Thoi gian hoat dong: ");
    Serial.print(currentMillis);
    Serial.print(" ms (~");
    Serial.print(currentMillis / 1000);
    Serial.println(" giay)");
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);

  Serial.println("========================================");
  Serial.println("ESP32 - Bai 3: LED Heartbeat + Serial");
  Serial.println("========================================");
}

void loop() {
  unsigned long currentMillis = millis();

  // Hai tiến trình chạy độc lập
  taskHeartbeatLED(currentMillis);
  taskSerialReport(currentMillis);
}
#include <Arduino.h>
#include <WiFi.h>
#include "config.h"

// Thời gian kiểm tra và tự kết nối lại Wi-Fi (nếu mất kết nối)
unsigned long lastCheckTime = 0;
const unsigned long checkInterval = 5000; // 5 giây kiểm tra 1 lần

void connectToWiFi() {
  Serial.println();
  Serial.print("[WiFi] Đang kết nối tới mạng: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startAttemptTime = millis();
  // Chờ tối đa 15 giây cho một lần thử kết nối ban đầu
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("[WiFi] Kết nối Wi-Fi thành công!");
    Serial.print("[WiFi] Địa chỉ IP được cấp: ");
    Serial.println(WiFi.localIP());
    Serial.print("[WiFi] Cường độ tín hiệu (RSSI): ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println("[WiFi] Kết nối Wi-Fi thất bại hoặc quá thời gian chờ (timeout). Sẽ thử lại sau.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=========================================");
  Serial.println(" Lab 05 - Bài 01: Thiết lập Wi-Fi cho ESP32");
  Serial.println("=========================================");

  connectToWiFi();
}

void loop() {
  unsigned long currentMillis = millis();

  // Kiểm tra trạng thái Wi-Fi định kỳ không chặn (non-blocking)
  if (currentMillis - lastCheckTime >= checkInterval) {
    lastCheckTime = currentMillis;

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("[WiFi] Cảnh báo: Mất kết nối Wi-Fi! Đang thực hiện kết nối lại...");
      WiFi.reconnect();
    } else {
      // In trạng thái liveness nhẹ nhàng mỗi chu kỳ
      Serial.print("[WiFi] Trạng thái: Đang kết nối | IP: ");
      Serial.println(WiFi.localIP());
    }
  }
}

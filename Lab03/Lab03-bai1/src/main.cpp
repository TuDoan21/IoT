#include <Arduino.h>
#include <WiFi.h>

// Mạng WiFi mô phỏng mặc định của Wokwi không có mật khẩu.
const char* ssid = "Wokwi-GUEST";
const char* password = "";

const unsigned long dotIntervalMs = 500;
unsigned long lastDotMs = 0;
bool wasConnected = false;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("ESP32 - WiFi Station Mode");
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  // ESP32 đóng vai trò thiết bị kết nối vào điểm truy cập WiFi.
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);
  lastDotMs = millis();
}

void loop() {
  const unsigned long now = millis();
  const bool isConnected = WiFi.status() == WL_CONNECTED;

  if (isConnected) {
    // Chỉ in thông tin một lần mỗi khi kết nối thành công.
    if (!wasConnected) {
      Serial.println();
      Serial.println("==============================");
      Serial.println("WIFI CONNECTED SUCCESS");
      Serial.println("==============================");
      Serial.print("SSID: ");
      Serial.println(WiFi.SSID());
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      Serial.print("Signal RSSI: ");
      Serial.print(WiFi.RSSI());
      Serial.println(" dBm");
      Serial.println("==============================");
    }
  } else {
    if (wasConnected) {
      Serial.println("WiFi disconnected. Reconnecting...");
      lastDotMs = now;
    }

    // Dùng millis() để in dấu chấm mỗi 500 ms mà không chặn vòng lặp.
    // Phép trừ unsigned vẫn đúng khi bộ đếm millis() tràn.
    if (now - lastDotMs >= dotIntervalMs) {
      Serial.print('.');
      lastDotMs = now;
    }
  }

  wasConnected = isConnected;
  // Nhường CPU trong thời gian rất ngắn cho các tác vụ nền.
  delay(1);
}

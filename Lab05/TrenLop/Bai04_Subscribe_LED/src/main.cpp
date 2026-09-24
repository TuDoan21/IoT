#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"

WiFiClient espClient;
PubSubClient mqttClient(espClient);

unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 5000;

// Trạng thái hiện tại của LED (false = OFF, true = ON)
bool ledState = false;

// Hàm callback nhận thông điệp MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("-----------------------------------------");
  Serial.print("[MQTT Callback] Nhận bản tin từ Topic: ");
  Serial.println(topic);

  // Đọc payload đúng theo chiều dài length (không giả định dữ liệu có ký tự kết thúc '\0')
  char messageBuffer[64];
  unsigned int copyLength = (length < sizeof(messageBuffer) - 1) ? length : (sizeof(messageBuffer) - 1);
  memcpy(messageBuffer, payload, copyLength);
  messageBuffer[copyLength] = '\0'; // Thêm ký tự kết thúc chuỗi an toàn

  Serial.print("[MQTT Callback] Lệnh nhận được: \"");
  Serial.print(messageBuffer);
  Serial.println("\"");

  // Xử lý lệnh ON / OFF
  if (strcmp(messageBuffer, "ON") == 0) {
    ledState = true;
    digitalWrite(LED_PIN, HIGH);
    Serial.println("[LED Control] -> Đã BẬT LED (GPIO " + String(LED_PIN) + " = HIGH)");
  } else if (strcmp(messageBuffer, "OFF") == 0) {
    ledState = false;
    digitalWrite(LED_PIN, LOW);
    Serial.println("[LED Control] -> Đã TẮT LED (GPIO " + String(LED_PIN) + " = LOW)");
  } else {
    Serial.print("[LED Control] CẢNH BÁO: Lệnh không hợp lệ! Giữ nguyên trạng thái LED (");
    Serial.print(ledState ? "ON" : "OFF");
    Serial.println("). Các lệnh hợp lệ là \"ON\" hoặc \"OFF\".");
  }
}

void connectToWiFi() {
  Serial.println();
  Serial.print("[WiFi] Đang kết nối tới: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 15000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("[WiFi] Đã kết nối! IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("[WiFi] Kết nối Wi-Fi thất bại. Sẽ tự động thử lại...");
  }
}

bool reconnectMQTT() {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }

  Serial.print("[MQTT] Đang kết nối lại tới broker ");
  Serial.print(MQTT_BROKER);
  Serial.print("...");

  if (mqttClient.connect(MQTT_CLIENT_ID)) {
    Serial.println(" THÀNH CÔNG!");

    // SAU MỖI LẦN KẾT NỐI LẠI, PHẢI SUBSCRIBE LẠI TOPIC CẦN NHẬN
    Serial.print("[MQTT] Đang subscribe topic: ");
    Serial.println(TOPIC_LED_CONTROL);
    bool subSuccess = mqttClient.subscribe(TOPIC_LED_CONTROL);
    if (subSuccess) {
      Serial.println("[MQTT] Subscribe topic thành công! Sẵn sàng nhận lệnh ON/OFF.");
    } else {
      Serial.println("[MQTT] CẢNH BÁO: Subscribe topic thất bại!");
    }
    return true;
  } else {
    Serial.print(" THẤT BẠI! Mã lỗi state = ");
    Serial.println(mqttClient.state());
    return false;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // Mặc định tắt LED khi khởi động

  Serial.println("=========================================");
  Serial.println(" Lab 05 - Bài 04: Subscribe và Điều Khiển LED");
  Serial.println("=========================================");

  connectToWiFi();
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(callback);
}

void loop() {
  // 1. Kiểm tra kết nối Wi-Fi
  if (WiFi.status() != WL_CONNECTED) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > reconnectInterval) {
      lastReconnectAttempt = now;
      WiFi.reconnect();
    }
    return;
  }

  // 2. Kiểm tra kết nối MQTT
  if (!mqttClient.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > reconnectInterval) {
      lastReconnectAttempt = now;
      reconnectMQTT();
    }
    return;
  }

  // 3. Duy trì MQTT loop để nhận và xử lý callback
  mqttClient.loop();
}

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"

WiFiClient espClient;
PubSubClient mqttClient(espClient);

unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 5000; // 5 giây thử kết nối lại 1 lần nếu mất kết nối

// Chuyển mã lỗi client.state() sang chuỗi giải thích có nghĩa
const char* getMqttStateDescription(int state) {
  switch (state) {
    case -4: return "MQTT_CONNECTION_TIMEOUT (Hết thời gian chờ kết nối máy chủ)";
    case -3: return "MQTT_CONNECTION_LOST (Mất kết nối mạng tới máy chủ)";
    case -2: return "MQTT_CONNECT_FAILED (Không thể khởi tạo kết nối mạng)";
    case -1: return "MQTT_DISCONNECTED (Đã ngắt kết nối)";
    case 0:  return "MQTT_CONNECTED (Đã kết nối thành công)";
    case 1:  return "MQTT_CONNECT_BAD_PROTOCOL (Phiên bản giao thức không tương thích)";
    case 2:  return "MQTT_CONNECT_BAD_CLIENT_ID (Client ID bị máy chủ từ chối)";
    case 3:  return "MQTT_CONNECT_UNAVAILABLE (Máy chủ MQTT hiện không khả dụng)";
    case 4:  return "MQTT_CONNECT_BAD_CREDENTIALS (Sai Username hoặc Password)";
    case 5:  return "MQTT_CONNECT_UNAUTHORIZED (Client không được cấp quyền truy cập)";
    default: return "Lỗi không xác định";
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
    Serial.println("[WiFi] Kết nối Wi-Fi chưa thành công. Sẽ tự động thử lại...");
  }
}

bool reconnectMQTT() {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }

  Serial.print("[MQTT] Đang thử kết nối tới broker ");
  Serial.print(MQTT_BROKER);
  Serial.print(":");
  Serial.print(MQTT_PORT);
  Serial.print(" với Client ID: ");
  Serial.println(MQTT_CLIENT_ID);

  if (mqttClient.connect(MQTT_CLIENT_ID)) {
    Serial.println("[MQTT] KẾT NỐI BROKER THÀNH CÔNG!");
    Serial.print("[MQTT] Trạng thái hiện tại: ");
    Serial.println(getMqttStateDescription(mqttClient.state()));
    return true;
  } else {
    int state = mqttClient.state();
    Serial.print("[MQTT] KẾT NỐI THẤT BẠI! Mã lỗi (state): ");
    Serial.print(state);
    Serial.print(" -> ");
    Serial.println(getMqttStateDescription(state));
    return false;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=========================================");
  Serial.println(" Lab 05 - Bài 02: Kết nối Public HiveMQ Broker");
  Serial.println("=========================================");

  connectToWiFi();
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
}

void loop() {
  // 1. Đảm bảo Wi-Fi luôn kết nối
  if (WiFi.status() != WL_CONNECTED) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > reconnectInterval) {
      lastReconnectAttempt = now;
      Serial.println("[WiFi] Mất Wi-Fi! Đang thử kết nối lại...");
      WiFi.reconnect();
    }
    return;
  }

  // 2. Đảm bảo MQTT luôn kết nối (không chặn vô hạn)
  if (!mqttClient.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > reconnectInterval) {
      lastReconnectAttempt = now;
      reconnectMQTT();
    }
  } else {
    // Duy trì giao tiếp MQTT
    mqttClient.loop();
  }
}

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"

WiFiClient espClient;
PubSubClient mqttClient(espClient);

unsigned long lastReconnectAttempt = 0;
unsigned long lastPublishTime = 0;
const unsigned long reconnectInterval = 5000;

// Giá trị nhiệt độ mẫu giả lập tăng giảm nhẹ để quan sát trực quan
float mockTemperature = 28.5;

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

  Serial.print("[MQTT] Đang thử kết nối tới broker ");
  Serial.print(MQTT_BROKER);
  Serial.print("...");

  if (mqttClient.connect(MQTT_CLIENT_ID)) {
    Serial.println(" THÀNH CÔNG!");
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

  Serial.println("=========================================");
  Serial.println(" Lab 05 - Bài 03: Publish Dữ Liệu Lên MQTT");
  Serial.println("=========================================");

  connectToWiFi();
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
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

  // 3. Duy trì MQTT loop
  mqttClient.loop();

  // 4. Publish dữ liệu định kỳ bằng millis()
  unsigned long currentMillis = millis();
  if (currentMillis - lastPublishTime >= PUBLISH_INTERVAL_MS) {
    lastPublishTime = currentMillis;

    // Chuẩn bị payload chuỗi mẫu (Lưu ý: Đây là dữ liệu mẫu mô phỏng, chưa phải cảm biến vật lý)
    // Thay đổi nhẹ giá trị mẫu để người học dễ phân biệt các gói tin mới
    mockTemperature += 0.2;
    if (mockTemperature > 32.0) {
      mockTemperature = 28.0;
    }

    char payload[64];
    snprintf(payload, sizeof(payload), "Temperature: %.1fC", mockTemperature);

    // Gửi bản tin lên broker
    bool success = mqttClient.publish(TOPIC_TEMPERATURE, payload);

    // In thông tin và kết quả ra Serial
    Serial.println("-----------------------------------------");
    Serial.print("[MQTT Publish] Topic   : ");
    Serial.println(TOPIC_TEMPERATURE);
    Serial.print("[MQTT Publish] Payload : ");
    Serial.println(payload);
    Serial.print("[MQTT Publish] Kết quả : ");
    if (success) {
      Serial.println("GỬI THÀNH CÔNG (SUCCESS)");
    } else {
      Serial.println("GỬI THẤT BẠI (FAILED)");
    }
    Serial.println("*(Lưu ý: Dữ liệu trên là chuỗi mẫu giả lập, chưa phải số đo từ cảm biến)*");
  }
}

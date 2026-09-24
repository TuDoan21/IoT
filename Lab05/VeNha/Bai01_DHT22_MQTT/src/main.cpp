#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include "config.h"

// Khởi tạo đối tượng cảm biến DHT22
DHT dht(DHT_PIN, DHT_TYPE);

WiFiClient espClient;
PubSubClient mqttClient(espClient);

unsigned long lastReconnectAttempt = 0;
unsigned long lastReadTime = 0;
const unsigned long reconnectInterval = 5000;

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

  Serial.println("==================================================");
  Serial.println(" Lab 05 - Về Nhà 01: Trạm Đo Nhiệt Độ & Độ Ẩm DHT22");
  Serial.println("==================================================");

  // Khởi động cảm biến DHT22
  dht.begin();
  Serial.println("[DHT22] Cảm biến DHT22 đã được khởi tạo trên chân GPIO " + String(DHT_PIN));

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

  // 4. Đọc cảm biến DHT22 và publish định kỳ 10 giây một lần
  unsigned long currentMillis = millis();
  if (currentMillis - lastReadTime >= DHT_READ_INTERVAL_MS) {
    lastReadTime = currentMillis;

    // Đọc độ ẩm (%) và nhiệt độ (°C)
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    // Kiểm tra tính hợp lệ của giá trị đọc từ DHT22
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("--------------------------------------------------");
      Serial.println("[DHT22 Error] Không thể đọc dữ liệu từ cảm biến DHT22!");
      Serial.println("-> Bỏ qua lần publish này để tránh phát tán dữ liệu lỗi lên broker.");
      return;
    }

    // Đóng gói dữ liệu dạng JSON hợp lệ: {"temp":26.5,"hum":80.0}
    JsonDocument doc;
    doc["temp"] = serialized(String(temperature, 1));
    doc["hum"]  = serialized(String(humidity, 1));

    char jsonBuffer[128];
    serializeJson(doc, jsonBuffer);

    // Publish lên broker MQTT
    bool pubSuccess = mqttClient.publish(TOPIC_ENV, jsonBuffer);

    // In chi tiết ra Serial Monitor
    Serial.println("--------------------------------------------------");
    Serial.print("[DHT22 Read]   Nhiệt độ: ");
    Serial.print(temperature, 1);
    Serial.print(" °C | Độ ẩm: ");
    Serial.print(humidity, 1);
    Serial.println(" %");
    Serial.print("[MQTT Publish] Topic   : ");
    Serial.println(TOPIC_ENV);
    Serial.print("[MQTT Publish] Payload : ");
    Serial.println(jsonBuffer);
    Serial.print("[MQTT Publish] Kết quả : ");
    Serial.println(pubSuccess ? "GỬI THÀNH CÔNG (SUCCESS)" : "GỬI THẤT BẠI (FAILED)");
  }
}

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"

WiFiClient espClient;
PubSubClient mqttClient(espClient);

unsigned long lastReconnectAttempt = 0;
unsigned long lastPublishTime = 0;
const unsigned long reconnectInterval = 5000;

// Trạng thái xuất bản dữ liệu
bool publishEnabled = true;
unsigned long publishInterval = DEFAULT_INTERVAL_MS;

// Hàm callback nhận lệnh điều khiển từ topic TOPIC_CMD
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("-----------------------------------------");
  Serial.print("[MQTT Callback] Nhận lệnh từ Topic: ");
  Serial.println(topic);

  char cmdBuffer[64];
  unsigned int copyLength = (length < sizeof(cmdBuffer) - 1) ? length : (sizeof(cmdBuffer) - 1);
  memcpy(cmdBuffer, payload, copyLength);
  cmdBuffer[copyLength] = '\0';

  Serial.print("[MQTT Callback] Nội dung lệnh: \"");
  Serial.print(cmdBuffer);
  Serial.println("\"");

  if (strcmp(cmdBuffer, "ON") == 0) {
    publishEnabled = true;
    Serial.println("[Command Response] -> ĐÃ BẬT gửi dữ liệu định kỳ.");
  } else if (strcmp(cmdBuffer, "OFF") == 0) {
    publishEnabled = false;
    Serial.println("[Command Response] -> ĐÃ TẠM DỪNG gửi dữ liệu (MQTT vẫn duy trì kết nối).");
  } else if (strncmp(cmdBuffer, "INTERVAL:", 9) == 0) {
    char* intervalStr = cmdBuffer + 9;
    
    // Kiểm tra xem toàn bộ phần sau INTERVAL: có phải là chữ số không
    bool isAllDigits = (strlen(intervalStr) > 0);
    for (size_t i = 0; i < strlen(intervalStr); i++) {
      if (!isdigit(intervalStr[i])) {
        isAllDigits = false;
        break;
      }
    }

    if (isAllDigits) {
      long newInterval = atol(intervalStr);
      if (newInterval >= 500) { // Giới hạn tối thiểu 500ms để tránh spam broker
        publishInterval = (unsigned long)newInterval;
        Serial.print("[Command Response] -> ĐÃ ĐỔI chu kỳ gửi dữ liệu thành: ");
        Serial.print(publishInterval);
        Serial.println(" ms.");
      } else {
        Serial.print("[Command Response] LỖI: Giá trị INTERVAL quá nhỏ (< 500 ms): ");
        Serial.println(newInterval);
      }
    } else {
      Serial.print("[Command Response] LỖI: Giá trị INTERVAL không phải là số hợp lệ: \"");
      Serial.print(intervalStr);
      Serial.println("\"");
    }
  } else {
    Serial.print("[Command Response] LỖI: Lệnh không được hỗ trợ! Lệnh hợp lệ: ON, OFF, INTERVAL:<ms>");
    Serial.println();
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

    // Subscribe lại topic lệnh sau mỗi lần kết nối lại
    Serial.print("[MQTT] Đang subscribe topic lệnh: ");
    Serial.println(TOPIC_CMD);
    if (mqttClient.subscribe(TOPIC_CMD)) {
      Serial.println("[MQTT] Subscribe topic lệnh THÀNH CÔNG!");
    } else {
      Serial.println("[MQTT] CẢNH BÁO: Subscribe topic lệnh THẤT BẠI!");
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

  // Khởi tạo chân ADC
  pinMode(ADC_PIN, INPUT);

  Serial.println("=========================================");
  Serial.println(" Lab 05 - Bài 05: Giao Tiếp Hai Chiều (ADC & CMD)");
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

  // 3. Duy trì MQTT loop liên tục (đảm bảo luôn tiếp nhận lệnh kể cả khi đang tạm dừng publish)
  mqttClient.loop();

  // 4. Publish dữ liệu định kỳ nếu publishEnabled == true
  unsigned long currentMillis = millis();
  if (currentMillis - lastPublishTime >= publishInterval) {
    lastPublishTime = currentMillis;

    // Đọc giá trị ADC thô từ chân ADC_PIN (GPIO 34 thuộc ADC1, dải giá trị 0 - 4095)
    int rawAdcValue = analogRead(ADC_PIN);

    if (publishEnabled) {
      char payload[32];
      snprintf(payload, sizeof(payload), "%d", rawAdcValue);

      bool pubSuccess = mqttClient.publish(TOPIC_DATA, payload);

      Serial.println("-----------------------------------------");
      Serial.print("[ADC Publish] Topic       : ");
      Serial.println(TOPIC_DATA);
      Serial.print("[ADC Publish] Giá trị thô : ");
      Serial.print(rawAdcValue);
      Serial.println(" (Raw ADC 12-bit: 0 - 4095)");
      Serial.print("[ADC Publish] Chu kỳ      : ");
      Serial.print(publishInterval);
      Serial.println(" ms");
      Serial.print("[ADC Publish] Trạng thái  : ĐANG BẬT (ACTIVE)");
      Serial.println();
      Serial.print("[ADC Publish] Kết quả     : ");
      Serial.println(pubSuccess ? "THÀNH CÔNG (SUCCESS)" : "THẤT BẠI (FAILED)");
    } else {
      // Khi đang tạm dừng (OFF), in trạng thái thông báo ra Serial để biết ESP32 vẫn hoạt động
      Serial.println("-----------------------------------------");
      Serial.print("[ADC Monitor] Trạng thái  : TẠM DỪNG (PAUSED)");
      Serial.print(" | Giá trị thô hiện tại: ");
      Serial.print(rawAdcValue);
      Serial.println(" | MQTT vẫn kết nối và lắng nghe lệnh.");
    }
  }
}

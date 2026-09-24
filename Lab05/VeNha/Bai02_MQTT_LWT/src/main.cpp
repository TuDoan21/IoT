#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"

WiFiClient espClient;
PubSubClient mqttClient(espClient);

unsigned long lastReconnectAttempt = 0;
unsigned long lastHeartbeat = 0;
const unsigned long reconnectInterval = 5000;
const unsigned long heartbeatInterval = 10000; // 10 giây in một dòng nhịp tim liveness

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

  Serial.println("--------------------------------------------------");
  Serial.print("[MQTT] Đang kết nối tới broker: ");
  Serial.println(MQTT_BROKER);
  Serial.print("[MQTT] Cấu hình LWT (Di chúc) : Topic=\"");
  Serial.print(TOPIC_STATUS);
  Serial.print("\", Payload=\"");
  Serial.print(LWT_PAYLOAD_OFFLINE);
  Serial.print("\", QoS=");
  Serial.print(LWT_QOS);
  Serial.print(", Retained=");
  Serial.println(LWT_RETAIN ? "true" : "false");

  // Thiết lập kết nối MQTT kèm thông điệp Last Will and Testament (LWT)
  // Broker sẽ lưu di chúc này. Nếu ESP32 bị ngắt kết nối đột ngột không qua bản tin DISCONNECT hợp lệ,
  // broker sẽ tự động phát bản tin này tới tất cả các subscriber đã đăng ký TOPIC_STATUS.
  bool connected = mqttClient.connect(
    MQTT_CLIENT_ID,
    TOPIC_STATUS,
    LWT_QOS,
    LWT_RETAIN,
    LWT_PAYLOAD_OFFLINE
  );

  if (connected) {
    Serial.println("[MQTT] KẾT NỐI BROKER THÀNH CÔNG!");
    
    // Ngay sau khi kết nối thành công, publish thông điệp "Online" với cờ retain = true lên cùng topic
    Serial.print("[MQTT] Đang publish trạng thái trực tuyến: \"");
    Serial.print(LWT_PAYLOAD_ONLINE);
    Serial.println("\" (retained = true)...");

    bool pubSuccess = mqttClient.publish(TOPIC_STATUS, LWT_PAYLOAD_ONLINE, LWT_RETAIN);
    if (pubSuccess) {
      Serial.println("[MQTT] -> Publish \"Online\" THÀNH CÔNG!");
    } else {
      Serial.println("[MQTT] -> Publish \"Online\" THẤT BẠI!");
    }

    Serial.println("*(Khi ESP32 mất nguồn hoặc mất mạng đột ngột, broker sẽ tự động phát \"Offline\")*");
    Serial.println("--------------------------------------------------");
    return true;
  } else {
    Serial.print("[MQTT] Kết nối THẤT BẠI! Mã lỗi state = ");
    Serial.println(mqttClient.state());
    return false;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("==================================================");
  Serial.println(" Lab 05 - Về Nhà 02: Last Will and Testament (LWT)");
  Serial.println("==================================================");

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

  // 4. In nhịp tim kiểm tra hoạt động bình thường
  unsigned long currentMillis = millis();
  if (currentMillis - lastHeartbeat >= heartbeatInterval) {
    lastHeartbeat = currentMillis;
    Serial.print("[Heartbeat] ESP32 đang hoạt động trực tuyến (Online) | Uptime: ");
    Serial.print(currentMillis / 1000);
    Serial.println("s");
  }
}

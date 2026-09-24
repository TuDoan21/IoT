#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "config.h"

WiFiClientSecure espClientSecure;
PubSubClient mqttClient(espClientSecure);

unsigned long lastReconnectAttempt = 0;
unsigned long lastPublishHeartbeat = 0;
const unsigned long reconnectInterval = 5000;
const unsigned long heartbeatInterval = 15000; // 15 giây gửi 1 bản tin kiểm tra lên Cloud

// Diễn giải mã lỗi client.state()
const char* getMqttStateDescription(int state) {
  switch (state) {
    case -4: return "MQTT_CONNECTION_TIMEOUT (Quá thời gian kết nối TLS/TCP)";
    case -3: return "MQTT_CONNECTION_LOST (Mất kết nối TLS)";
    case -2: return "MQTT_CONNECT_FAILED (Không thể khởi tạo kết nối TLS hoặc sai Host/Port/CA)";
    case -1: return "MQTT_DISCONNECTED (Đã ngắt kết nối)";
    case 0:  return "MQTT_CONNECTED (Kết nối TLS và xác thực thành công)";
    case 1:  return "MQTT_CONNECT_BAD_PROTOCOL (Sai giao thức MQTT)";
    case 2:  return "MQTT_CONNECT_BAD_CLIENT_ID (Client ID bị từ chối)";
    case 3:  return "MQTT_CONNECT_UNAVAILABLE (Cluster Cloud hiện không khả dụng)";
    case 4:  return "MQTT_CONNECT_BAD_CREDENTIALS (Sai Username hoặc Password trên HiveMQ Cloud)";
    case 5:  return "MQTT_CONNECT_UNAUTHORIZED (Tài khoản không có quyền truy cập topic)";
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
    Serial.println("[WiFi] Kết nối Wi-Fi thất bại. Sẽ tự động thử lại...");
  }
}

bool reconnectMQTT() {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }

  Serial.println("--------------------------------------------------");
  Serial.print("[TLS/MQTT] Đang thử kết nối Cluster HiveMQ Cloud: ");
  Serial.println(MQTT_BROKER);
  Serial.print("[TLS/MQTT] Cổng TLS: ");
  Serial.print(MQTT_PORT);
  Serial.print(" | Client ID: ");
  Serial.println(MQTT_CLIENT_ID);
  Serial.print("[TLS/MQTT] Username: ");
  Serial.println(MQTT_USERNAME);
  Serial.println("[TLS/MQTT] Password: [ĐÃ ẨN ĐỂ BẢO MẬT]");

  // Thử kết nối kèm Username & Password
  bool connected = mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD);

  if (connected) {
    Serial.println("[TLS/MQTT] KẾT NỐI VÀ XÁC THỰC HIVEMQ CLOUD THÀNH CÔNG!");
    
    // Publish bản tin kiểm thử sau khi kết nối thành công
    const char* testMsg = "ESP32 connected";
    bool pubSuccess = mqttClient.publish(TOPIC_CLOUD_STATUS, testMsg);

    Serial.print("[TLS/MQTT Publish] Topic   : ");
    Serial.println(TOPIC_CLOUD_STATUS);
    Serial.print("[TLS/MQTT Publish] Payload : ");
    Serial.println(testMsg);
    Serial.print("[TLS/MQTT Publish] Kết quả : ");
    Serial.println(pubSuccess ? "THÀNH CÔNG (SUCCESS)" : "THẤT BẠI (FAILED)");
    Serial.println("--------------------------------------------------");
    return true;
  } else {
    int errState = mqttClient.state();
    Serial.print("[TLS/MQTT] KẾT NỐI THẤT BẠI! Mã lỗi state = ");
    Serial.print(errState);
    Serial.print(" -> ");
    Serial.println(getMqttStateDescription(errState));

    // Hướng dẫn chẩn đoán lỗi chi tiết
    Serial.println(">>> HƯỚNG DẪN KIỂM TRA CHẨN ĐOÁN LỖI:");
    if (errState == 4 || errState == 5) {
      Serial.println(" -> LỖI XÁC THỰC: Kiểm tra lại MQTT_USERNAME và MQTT_PASSWORD trong config.h.");
      Serial.println(" -> Đảm bảo tài khoản đã được cấp quyền trong Console HiveMQ Cloud -> Access Management.");
    } else if (errState == -2 || errState == -4) {
      Serial.println(" -> LỖI KẾT NỐI TLS / MẠNG: ");
      Serial.println("    1. Kiểm tra URL Broker (phải là xxx.s1.eu.hivemq.cloud, không chứa mqtt:// hoặc https://).");
      Serial.println("    2. Kiểm tra cổng (phải là 8883, HiveMQ Cloud không hỗ trợ cổng không mã hóa 1883).");
      Serial.println("    3. Kiểm tra chứng chỉ gốc HIVEMQ_ROOT_CA trong config.h.");
      Serial.println("    4. Đảm bảo ESP32 có kết nối Internet thật để phân giải DNS của Cloud.");
    }
    Serial.println("--------------------------------------------------");
    return false;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("==========================================================");
  Serial.println(" Lab 05 - Về Nhà 03: Kết Nối HiveMQ Cloud (TLS Encrypted)");
  Serial.println("==========================================================");

  // Cấu hình chứng chỉ TLS Root CA (ISRG Root X1 của Let's Encrypt)
  // Tuyệt đối không dùng setInsecure() làm mặc định để đảm bảo an toàn kết nối
  espClientSecure.setCACert(HIVEMQ_ROOT_CA);
  Serial.println("[TLS] Đã nạp chứng chỉ Root CA ISRG Root X1 để xác thực máy chủ Cloud.");

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

  // 2. Kiểm tra kết nối MQTT Cloud
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

  // 4. Gửi heartbeat định kỳ lên topic cloud status
  unsigned long currentMillis = millis();
  if (currentMillis - lastPublishHeartbeat >= heartbeatInterval) {
    lastPublishHeartbeat = currentMillis;

    char hbMsg[64];
    snprintf(hbMsg, sizeof(hbMsg), "ESP32 TLS alive (Uptime: %lus)", currentMillis / 1000);
    mqttClient.publish(TOPIC_CLOUD_STATUS, hbMsg);

    Serial.print("[Cloud Heartbeat] Đã gửi: ");
    Serial.println(hbMsg);
  }
}

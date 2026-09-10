#include <Arduino.h>
#include <WiFi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#define TEST_DISCONNECT true

const char* ssid = "Wokwi-GUEST";
const char* password = "";
constexpr uint32_t RETRY_MS = 5000;
constexpr uint32_t STATUS_MS = 10000;
constexpr uint32_t TEST_AFTER_MS = 15000;

uint32_t lastRetryMs = 0;
uint32_t lastStatusMs = 0;
uint32_t connectedAtMs = 0;
uint32_t retryCount = 0;
bool connected = false;
bool testDone = false;

// Callback chạy trên task riêng: chuyển sự kiện qua queue về loop().
struct WiFiMessage {
  WiFiEvent_t event;
  uint8_t reason;
};
QueueHandle_t wifiEvents = nullptr;

void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
  if (event != ARDUINO_EVENT_WIFI_STA_START &&
      event != ARDUINO_EVENT_WIFI_STA_CONNECTED &&
      event != ARDUINO_EVENT_WIFI_STA_GOT_IP &&
      event != ARDUINO_EVENT_WIFI_STA_DISCONNECTED &&
      event != ARDUINO_EVENT_WIFI_STA_LOST_IP) {
    return;
  }
  WiFiMessage message{event, 0};
  if (event == ARDUINO_EVENT_WIFI_STA_DISCONNECTED) {
    message.reason = info.wifi_sta_disconnected.reason;
  }
  // Không chờ trong callback. loop() còn kiểm tra trạng thái dự phòng.
  xQueueSend(wifiEvents, &message, 0);
}

void reportConnected(uint32_t now) {
  connected = true;
  connectedAtMs = now;
  Serial.println("\n================================");
  Serial.println("     WIFI CONNECTED SUCCESS");
  Serial.println("================================");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
  Serial.printf("Retry Count: %lu\n", (unsigned long)retryCount);
  Serial.println("================================");
  // In số lần retry của đợt vừa rồi trước khi đặt lại về 0.
  retryCount = 0;
}

void reportDisconnected(uint32_t now) {
  // Chỉ bắt đầu lại mốc chờ khi vừa mất một kết nối đang hoạt động.
  // Retry thất bại không dời mốc, tránh chờ vô hạn.
  if (connected) {
    lastRetryMs = now;
  }
  connected = false;
  Serial.println("\n================================");
  Serial.println("       WIFI DISCONNECTED");
  Serial.println("================================");
  Serial.println("Waiting for reconnect... Retry interval: 5 seconds.");
  Serial.println("================================");
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n[BOOT] ESP32 - Bai 2: WiFi Auto-Reconnect");
  wifiEvents = xQueueCreate(16, sizeof(WiFiMessage));
  if (wifiEvents == nullptr) {
    Serial.println("[ERROR] Cannot create WiFi event queue.");
    return;
  }

  WiFi.onEvent(onWiFiEvent);
  WiFi.mode(WIFI_STA);
  // Tự quản lý retry để giữ khoảng cách 5 giây giữa các lần thử.
  WiFi.setAutoReconnect(false);
  Serial.printf("Connecting to WiFi: %s\n", ssid);
  lastRetryMs = millis();
  // Wokwi-GUEST dùng kênh 6, giúp bỏ qua bước quét toàn bộ kênh.
  WiFi.begin(ssid, password, 6);
}

void loop() {
  if (wifiEvents == nullptr) {
    return;
  }
  const uint32_t now = millis();
  WiFiMessage message;
  // Xử lý tối đa 16 sự kiện mỗi vòng, không chờ sự kiện hoặc WiFi.
  for (uint8_t i = 0; i < 16 &&
       xQueueReceive(wifiEvents, &message, 0) == pdTRUE; ++i) {
    switch (message.event) {
      case ARDUINO_EVENT_WIFI_STA_START:
        Serial.println("[EVENT] STA_START: WiFi Station started.");
        break;
      case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        Serial.println("[EVENT] STA_CONNECTED: Access Point connected, waiting for IP.");
        break;
      case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        Serial.println("[EVENT] STA_GOT_IP");
        if (!connected && WiFi.status() == WL_CONNECTED) {
          reportConnected(now);
        }
        break;
      case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        Serial.printf("[EVENT] STA_DISCONNECTED: reason=%u\n", message.reason);
        reportDisconnected(now);
        break;
      case ARDUINO_EVENT_WIFI_STA_LOST_IP:
        Serial.println("[EVENT] STA_LOST_IP");
        reportDisconnected(now);
        break;
      default:
        break;
    }
  }

  // Đối chiếu trạng thái thực tế nếu sự kiện bị bỏ lỡ khi queue đầy.
  const bool hasIP = WiFi.status() == WL_CONNECTED &&
                     WiFi.localIP() != IPAddress(0, 0, 0, 0);
  if (hasIP && !connected) {
    reportConnected(now);
  } else if (!hasIP && connected) {
    reportDisconnected(now);
  }

  // Phép trừ unsigned vẫn đúng khi millis() tràn bộ đếm.
  if (!connected && now - lastRetryMs >= RETRY_MS) {
    lastRetryMs = now;
    ++retryCount;
    Serial.printf("[%lu ms] Retry WiFi connection...\n", (unsigned long)now);
    Serial.printf("Retry attempt: %lu\n", (unsigned long)retryCount);
    // Dùng lại SSID/mật khẩu từ begin(), không chờ kết nối thành công.
    if (!WiFi.reconnect()) {
      Serial.println("[RETRY] Request failed; will try again in 5 seconds.");
    }
  }

  // Chỉ demo một lần mỗi lần khởi động; không đặt lại testDone khi reconnect.
  if (TEST_DISCONNECT && connected && !testDone &&
      now - connectedAtMs >= TEST_AFTER_MS) {
    testDone = true;
    Serial.println("[TEST] Disconnect WiFi once after 15 seconds with IP.");
    // Giữ radio và cấu hình AP để cơ chế retry tự kết nối lại.
    if (!WiFi.disconnect(false, false)) {
      Serial.println("[TEST] Disconnect request failed.");
    }
  }

  if (now - lastStatusMs >= STATUS_MS) {
    lastStatusMs = now;
    Serial.printf("\n[STATUS] Uptime: %lu ms\n", (unsigned long)now);
    if (connected) {
      Serial.println("WiFi: CONNECTED");
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());
      Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
    } else {
      Serial.println("WiFi: DISCONNECTED\nWaiting for reconnect...");
    }
    Serial.printf("Retry counter: %lu\n", (unsigned long)retryCount);
  }
}
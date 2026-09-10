#include <Arduino.h>
#include <WiFi.h>
#include <time.h>

/*
 * BÀI 4: ĐỒNG BỘ THỜI GIAN THỰC QUA NTP SERVER
 * Tác giả: Đoàn Tuấn Tú
 *
 * Mục tiêu:
 * - Kết nối WiFi (Wokwi-GUEST).
 * - Sử dụng API tích hợp configTime() để lấy thời gian từ NTP Server.
 * - Cấu hình múi giờ Việt Nam (UTC+7, gmtOffset_sec = 25200, daylightOffset_sec = 0).
 * - Sử dụng struct tm và getLocalTime() để đọc thời gian chuẩn.
 * - Định kỳ mỗi 1 giây in Date và Time qua Serial Monitor bằng millis() (không dùng delay dài).
 */

const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Cấu hình NTP Server và Múi giờ Việt Nam (UTC+7)
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.google.com";
const long  gmtOffset_sec = 7 * 3600;      // UTC+7: 7 giờ * 3600 giây = 25200 giây
const int   daylightOffset_sec = 0;        // Việt Nam không có giờ mùa hè (DST)

bool ntpSynced = false;
unsigned long lastDisplayMs = 0;
const unsigned long displayIntervalMs = 1000;

void printCurrentTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Waiting for NTP synchronization...");
    return;
  }

  // Định dạng ngày: DD/MM/YYYY và giờ: HH:MM:SS
  char dateStr[20];
  char timeStr[20];
  strftime(dateStr, sizeof(dateStr), "%d/%m/%Y", &timeinfo);
  strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);

  Serial.print("Date: ");
  Serial.print(dateStr);
  Serial.print(" | Time: ");
  Serial.println(timeStr);
}

void setup() {
  Serial.begin(115200);
  delay(200);

  Serial.println();
  Serial.println("==========================================");
  Serial.println("   ESP32 - NTP TIME SYNCHRONIZATION       ");
  Serial.println("==========================================");
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Đợi kết nối WiFi (dùng millis timeout để không treo)
  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 10000) {
    delay(500);
    Serial.print('.');
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected successfully!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi connection failed! Retrying in loop...");
  }

  // Khởi động đồng bộ NTP
  Serial.println("Initializing NTP configTime (UTC+7)...");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
}

void loop() {
  unsigned long now = millis();

  // Kiểm tra trạng thái đồng bộ NTP nếu chưa hoàn thành
  if (!ntpSynced) {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 100)) {
      ntpSynced = true;
      Serial.println();
      Serial.println("==========================================");
      Serial.println("  NTP synchronized successfully!          ");
      Serial.println("==========================================");
    } else {
      if (now - lastDisplayMs >= displayIntervalMs) {
        lastDisplayMs = now;
        Serial.println("Waiting for NTP synchronization...");
      }
      return;
    }
  }

  // Khi đã đồng bộ, in ngày giờ mỗi 1 giây bằng millis()
  if (now - lastDisplayMs >= displayIntervalMs) {
    lastDisplayMs = now;
    printCurrentTime();
  }

  delay(5);
}

#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <esp_sntp.h>

/*
 * BÀI 7: TỰ ĐỒNG BỘ LẠI NTP ĐỊNH KỲ
 * Tác giả: Đoàn Tuấn Tú
 *
 * Mục tiêu:
 * - Khởi tạo đồng bộ NTP lần đầu khi ESP32 khởi động.
 * - Duy trì đồng hồ nội bộ chính xác từng giây.
 * - Định kỳ tự động đồng bộ lại NTP (Resync) để bù độ trôi đồng hồ (drift).
 * - Tính toán và hiển thị thời gian trước/sau sync cùng độ lệch theo giây (difftime).
 * - Có cờ DEMO_MODE (30 giây thay vì 6 giờ) để dễ dàng kiểm thử trên Wokwi.
 * - Xử lý khi mất kết nối WiFi: Chờ có lại WiFi mới thực hiện resync, không treo CPU.
 */

#define DEMO_MODE true

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.google.com";
const long  gmtOffset_sec = 7 * 3600;      // UTC+7
const int   daylightOffset_sec = 0;

#if DEMO_MODE
  const unsigned long resyncIntervalMs = 30000;  // 30 giây trong chế độ Demo
#else
  const unsigned long resyncIntervalMs = 6UL * 3600UL * 1000UL; // 6 giờ trong thực tế
#endif

unsigned long lastResyncMs = 0;
unsigned long lastDisplayMs = 0;
bool initialSynced = false;

void formatTimeStr(time_t t, char* buffer, size_t bufSize) {
  struct tm* ti = localtime(&t);
  strftime(buffer, bufSize, "%H:%M:%S", ti);
}

void performNtpResync() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[NTP RESYNC] WiFi disconnected! Dang cho ket noi lai de resync...");
    return;
  }

  time_t beforeSync = time(nullptr);
  char timeBeforeStr[20];
  formatTimeStr(beforeSync, timeBeforeStr, sizeof(timeBeforeStr));

  Serial.println();
  Serial.println("================================");
  Serial.println("       NTP RESYNC               ");
  Serial.println("================================");
  Serial.print("Time before sync: ");
  Serial.println(timeBeforeStr);
  Serial.println("Synchronizing...");

  // Đồng bộ lại qua configTime
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

  // Đợi cập nhật trong thời gian ngắn (tối đa 3 giây)
  struct tm timeinfo;
  getLocalTime(&timeinfo, 3000);

  time_t afterSync = time(nullptr);
  char timeAfterStr[20];
  formatTimeStr(afterSync, timeAfterStr, sizeof(timeAfterStr));

  double diffSeconds = difftime(afterSync, beforeSync);

  Serial.print("Time after sync: ");
  Serial.println(timeAfterStr);
  Serial.print("Difference: ");
  Serial.print((long)diffSeconds);
  Serial.println(" second(s)");
  Serial.println("================================");
  Serial.println();

  lastResyncMs = millis();
}

void setup() {
  Serial.begin(115200);
  delay(200);

  Serial.println();
  Serial.println("==========================================");
  Serial.println("   ESP32 - PERIODIC NTP RESYNCHRONIZATION ");
  Serial.println("==========================================");
#if DEMO_MODE
  Serial.println("Mode: DEMO_MODE (Resync every 30 seconds)");
#else
  Serial.println("Mode: REAL_MODE (Resync every 6 hours)");
#endif
  Serial.println("==========================================");

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

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
    Serial.println("\nWiFi connection pending. Will retry in loop.");
  }

  // Khởi tạo đồng bộ NTP lần đầu
  Serial.println("\nInitial NTP synchronization...");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 5000)) {
    initialSynced = true;
    Serial.println("Initial NTP sync successful!");
  } else {
    Serial.println("Initial NTP sync waiting for time...");
  }

  lastResyncMs = millis();
  lastDisplayMs = millis();
}

void loop() {
  unsigned long now = millis();

  // Kiểm tra hoàn tất sync lần đầu nếu lúc boot chưa kịp có
  if (!initialSynced) {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 100)) {
      initialSynced = true;
      Serial.println("Initial NTP sync successful!");
      lastResyncMs = now;
    }
  }

  // In thời gian thực mỗi 1 giây
  if (now - lastDisplayMs >= 1000) {
    lastDisplayMs = now;

    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 50)) {
      char dateStr[20];
      char timeStr[20];
      strftime(dateStr, sizeof(dateStr), "%d/%m/%Y", &timeinfo);
      strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);

      Serial.print("Date: ");
      Serial.print(dateStr);
      Serial.print(" | Time: ");
      Serial.print(timeStr);
      Serial.print(" | WiFi: ");
      Serial.println(WiFi.status() == WL_CONNECTED ? "CONNECTED" : "DISCONNECTED");
    } else {
      Serial.println("Waiting for NTP synchronization...");
    }
  }

  // Kiểm tra thời điểm tự động đồng bộ lại NTP
  if (initialSynced && (now - lastResyncMs >= resyncIntervalMs)) {
    performNtpResync();
  }

  delay(10);
}

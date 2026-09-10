#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>

/*
 * BÀI 5: HIỂN THỊ THỜI GIAN NTP LÊN OLED SSD1306
 * Tác giả: Đoàn Tuấn Tú
 *
 * Phần cứng:
 * - ESP32 DevKit v1
 * - Màn hình OLED SSD1306 128x64 I2C (Địa chỉ 0x3C)
 * - Nối chân: SDA -> GPIO21, SCL -> GPIO22, VCC -> 3V3, GND -> GND
 *
 * Chức năng:
 * - Kết nối WiFi (Wokwi-GUEST).
 * - Đồng bộ thời gian NTP (UTC+7).
 * - Hiển thị Date, Time và trạng thái WiFi lên màn hình OLED và Serial.
 * - Cập nhật đồng hồ mỗi 1 giây bằng millis(), không flicker màn hình.
 */

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Cấu hình NTP Server
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.google.com";
const long  gmtOffset_sec = 7 * 3600;      // UTC+7 (25200 giây)
const int   daylightOffset_sec = 0;

bool ntpSynced = false;
unsigned long lastUpdateMs = 0;
const unsigned long updateIntervalMs = 1000;

// Hàm hiển thị thông tin lên OLED mà không gây giật màn hình
void updateOLED(const char* dateStr, const char* timeStr, bool wifiOk) {
  display.clearDisplay();

  // Tiêu đề nhỏ
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(18, 2);
  display.print("--- NTP CLOCK ---");

  // Hiển thị ngày
  display.setTextSize(1);
  display.setCursor(32, 16);
  display.print(dateStr);

  // Hiển thị giờ (font to nổi bật)
  display.setTextSize(2);
  display.setCursor(16, 30);
  display.print(timeStr);

  // Hiển thị trạng thái WiFi
  display.setTextSize(1);
  display.setCursor(32, 52);
  if (wifiOk) {
    display.print("WiFi: OK");
  } else {
    display.print("WiFi: LOST");
  }

  display.display();
}

void setup() {
  Serial.begin(115200);
  delay(200);

  Serial.println();
  Serial.println("==========================================");
  Serial.println("   ESP32 - NTP CLOCK WITH OLED SSD1306   ");
  Serial.println("==========================================");

  // Khởi động I2C và OLED
  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("[ERROR] SSD1306 allocation failed! Kiem tra dia chi 0x3C va ket noi I2C."));
    for (;;); // Dừng nếu không tìm thấy màn hình
  }

  // Màn hình khởi động
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println("Connecting WiFi...");
  display.setCursor(10, 36);
  display.println("SSID: Wokwi-GUEST");
  display.display();

  // Kết nối WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 10000) {
    delay(500);
    Serial.print('.');
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected successfully!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    display.clearDisplay();
    display.setCursor(10, 20);
    display.println("WiFi Connected!");
    display.setCursor(10, 36);
    display.println("Syncing NTP...");
    display.display();
  } else {
    Serial.println("\nWiFi connection timeout. Retrying in background...");
  }

  // Cấu hình NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
}

void loop() {
  unsigned long now = millis();
  bool isWifiConnected = (WiFi.status() == WL_CONNECTED);

  // Đọc thời gian thực
  struct tm timeinfo;
  bool gotTime = getLocalTime(&timeinfo, 100);

  if (gotTime && !ntpSynced) {
    ntpSynced = true;
    Serial.println("\nNTP synchronized successfully!");
  }

  // Cập nhật màn hình và Serial mỗi giây (non-blocking)
  if (now - lastUpdateMs >= updateIntervalMs) {
    lastUpdateMs = now;

    char dateStr[20];
    char timeStr[20];

    if (gotTime) {
      strftime(dateStr, sizeof(dateStr), "%d/%m/%Y", &timeinfo);
      strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
    } else {
      strcpy(dateStr, "--/--/----");
      strcpy(timeStr, "--:--:--");
    }

    // Cập nhật OLED
    updateOLED(dateStr, timeStr, isWifiConnected);

    // In Serial Monitor
    Serial.print("Date: ");
    Serial.print(dateStr);
    Serial.print(" | Time: ");
    Serial.print(timeStr);
    Serial.print(" | WiFi: ");
    Serial.println(isWifiConnected ? "OK" : "LOST");
  }

  delay(5);
}

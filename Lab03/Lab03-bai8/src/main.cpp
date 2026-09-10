#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>
#include <time.h>

/*
 * BÀI 8: ĐỒNG HỒ THÔNG MINH NTP CÓ XỬ LÝ MẤT MẠNG (BÀI TỔNG HỢP)
 * Tác giả: Đoàn Tuấn Tú
 *
 * Phần cứng:
 * - ESP32 DevKit v1
 * - OLED SSD1306 128x64 I2C (SDA -> GPIO21, SCL -> GPIO22, 0x3C)
 *
 * Tính năng chính:
 * 1. Khởi động đọc WiFi từ Flash (Preferences), fallback 'Wokwi-GUEST' khi bật WOKWI_MODE.
 * 2. Kết nối WiFi và đồng bộ thời gian NTP (UTC+7).
 * 3. Hiển thị thông tin ngày giờ và trạng thái mạng trên OLED SSD1306.
 * 4. Xử lý mất mạng:
 *    - Không dừng đồng hồ, clock nội ESP32 vẫn đếm tiếp tục chính xác.
 *    - Không restart ESP32.
 *    - Tự động kiểm tra và reconnect ngầm mỗi 5 giây bằng millis().
 * 5. Khi có lại mạng:
 *    - Tự động phát hiện mạng phục hồi.
 *    - Tự động đồng bộ lại NTP (Resync).
 *    - Cập nhật OLED từ OFFLINE -> ONLINE.
 * 6. Chế độ TEST_DISCONNECT:
 *    - Tự động ngắt kết nối WiFi 1 lần sau 20s để demo trực quan trên Wokwi.
 */

#define WOKWI_MODE true
#define TEST_DISCONNECT true

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Preferences preferences;

// Cấu hình NTP Server
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.google.com";
const long  gmtOffset_sec = 7 * 3600; // UTC+7
const int   daylightOffset_sec = 0;

String wifiSSID = "Wokwi-GUEST";
String wifiPass = "";

bool isConnected = false;
bool wasConnected = false;
bool ntpSynced = false;

unsigned long connectedStartTime = 0;
unsigned long lastOledUpdateMs = 0;
unsigned long lastReconnectAttemptMs = 0;
bool testDisconnectDone = false;

void updateOLED(const char* dateStr, const char* timeStr, bool online) {
  display.clearDisplay();

  // Dòng ngày (cỡ chữ 1)
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(32, 4);
  display.print(dateStr);

  // Dòng giờ to nổi bật (cỡ chữ 2)
  display.setTextSize(2);
  display.setCursor(16, 18);
  display.print(timeStr);

  // Đường kẻ phân cách
  display.drawLine(0, 38, 127, 38, SSD1306_WHITE);

  // Vùng thông tin trạng thái mạng (cỡ chữ 1)
  display.setTextSize(1);
  if (online) {
    display.setCursor(10, 43);
    display.print("WiFi: ONLINE");
    display.setCursor(10, 53);
    display.print("NTP : SYNC");
  } else {
    display.setCursor(10, 43);
    display.print("WiFi: OFFLINE");
    display.setCursor(10, 53);
    display.print("! NO NETWORK");
  }

  display.display();
}

void syncNTP() {
  Serial.println("Synchronizing NTP time...");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 3000)) {
    ntpSynced = true;
    Serial.println("NTP synchronization successful.");
  } else {
    Serial.println("Waiting for NTP time...");
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);

  Serial.println();
  Serial.println("==========================================");
  Serial.println("   ESP32 SMART CLOCK WITH NETWORK WATCH   ");
  Serial.println("==========================================");

  // Khởi tạo OLED
  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("[ERROR] OLED SSD1306 not found!"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println("Smart Clock Booting");
  display.setCursor(10, 36);
  display.println("Loading config...");
  display.display();

  // Đọc WiFi từ Preferences
  preferences.begin("clock-cfg", false);
  String savedSSID = preferences.getString("ssid", "");
  String savedPass = preferences.getString("pass", "");

#if WOKWI_MODE
  if (savedSSID.length() == 0) {
    wifiSSID = "Wokwi-GUEST";
    wifiPass = "";
  } else {
    wifiSSID = savedSSID;
    wifiPass = savedPass;
  }
#else
  wifiSSID = savedSSID;
  wifiPass = savedPass;
#endif

  Serial.print("Connecting to WiFi: ");
  Serial.println(wifiSSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSSID.c_str(), wifiPass.c_str());

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 10000) {
    delay(500);
    Serial.print('.');
  }

  if (WiFi.status() == WL_CONNECTED) {
    isConnected = true;
    wasConnected = true;
    connectedStartTime = millis();

    Serial.println("\nWiFi connected successfully!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    syncNTP();
  } else {
    Serial.println("\nInitial WiFi connection timeout. Entering offline clock mode.");
  }
}

void loop() {
  unsigned long now = millis();
  isConnected = (WiFi.status() == WL_CONNECTED);

  // 1. Phát hiện sự kiện chuyển đổi trạng thái WiFi
  if (isConnected && !wasConnected) {
    // WiFi vừa phục hồi
    Serial.println();
    Serial.println("==========================================");
    Serial.println("WiFi connection restored.");
    Serial.println("Resynchronizing NTP...");
    syncNTP();
    Serial.println("==========================================");
    connectedStartTime = now;
  } else if (!isConnected && wasConnected) {
    // WiFi vừa mất
    Serial.println();
    Serial.println("[WARNING] WiFi lost! Continuing clock on internal RTC.");
    lastReconnectAttemptMs = now;
  }
  wasConnected = isConnected;

  // 2. Chức năng TEST_DISCONNECT mô phỏng ngắt mạng trên Wokwi
#if TEST_DISCONNECT
  if (!testDisconnectDone && isConnected && (now - connectedStartTime >= 20000)) {
    testDisconnectDone = true;
    Serial.println();
    Serial.println("==========================================");
    Serial.println("[TEST_DISCONNECT] Tự động ngắt WiFi 1 lần");
    Serial.println("để kiểm tra đồng hồ và cơ chế reconnect...");
    Serial.println("==========================================");
    WiFi.disconnect();
    isConnected = false;
    wasConnected = false;
    lastReconnectAttemptMs = now;
  }
#endif

  // 3. Cơ chế tự động kết nối lại định kỳ mỗi 5 giây khi mất mạng (Non-blocking)
  if (!isConnected) {
    if (now - lastReconnectAttemptMs >= 5000) {
      lastReconnectAttemptMs = now;
      Serial.println("[RECONNECT] Đang thử kết nối lại WiFi...");
      WiFi.begin(wifiSSID.c_str(), wifiPass.c_str());
    }
  }

  // 4. Cập nhật đồng hồ lên OLED và Serial mỗi 1 giây (Đồng hồ vẫn chạy liên tục dù mất WiFi)
  if (now - lastOledUpdateMs >= 1000) {
    lastOledUpdateMs = now;

    struct tm timeinfo;
    bool hasTime = getLocalTime(&timeinfo, 50);

    char dateStr[20];
    char timeStr[20];

    if (hasTime) {
      strftime(dateStr, sizeof(dateStr), "%d/%m/%Y", &timeinfo);
      strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
    } else {
      strcpy(dateStr, "10/09/2026");
      strcpy(timeStr, "--:--:--");
    }

    // Cập nhật màn hình OLED
    updateOLED(dateStr, timeStr, isConnected);

    // In Serial Monitor
    Serial.print(dateStr);
    Serial.print(" - ");
    Serial.print(timeStr);
    Serial.print(" | WiFi: ");
    Serial.print(isConnected ? "ONLINE" : "OFFLINE");
    Serial.print(" | NTP: ");
    Serial.println(ntpSynced ? "SYNC" : "NO_SYNC");
  }

  delay(10);
}

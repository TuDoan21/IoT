#include <Arduino.h>
#include <WiFi.h>

/*
 * BÀI 6: WATCHDOG WIFI + KIỂM TRA INTERNET
 * Tác giả: Đoàn Tuấn Tú
 *
 * Mục tiêu:
 * - Kiểm tra kết nối Internet thực sự (không chỉ trạng thái WiFi).
 * - Sử dụng WiFiClient để kiểm tra kết nối TCP đến server công cộng (google.com hoặc 8.8.8.8).
 * - Đếm số lần thất bại liên tiếp: nếu lỗi 3 lần liên tiếp, khởi động lại ESP32 bằng ESP.restart().
 * - Có chế độ DEMO_MODE để dễ dàng kiểm tra trên Wokwi (chu kỳ 15 giây thay vì 5 phút).
 * - Hỗ trợ nhập 'fail' trên Serial để mô phỏng lỗi rớt mạng kiểm tra tính năng restart sau 3 lần.
 */

#define DEMO_MODE true

const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Cấu hình máy chủ kiểm tra Internet
const char* testHost = "www.google.com";
const uint16_t testPort = 80;

#if DEMO_MODE
  const unsigned long checkIntervalMs = 15000; // 15 giây trong chế độ Demo
#else
  const unsigned long checkIntervalMs = 300000; // 5 phút trong chế độ thực tế
#endif

int failureCount = 0;
const int MAX_FAILURES = 3;
unsigned long lastCheckMs = 0;
bool simulateFailure = false; // Cờ mô phỏng lỗi kiểm tra qua Serial

bool checkInternetConnection() {
  if (simulateFailure) {
    Serial.println("[SIMULATION] Dang mo phong mat ket noi Internet!");
    return false;
  }

  // Nếu WiFi chưa kết nối thì chắc chắn không có Internet
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }

  WiFiClient client;
  client.setTimeout(3000); // 3 giây timeout

  // Thử kết nối TCP đến testHost port 80
  if (client.connect(testHost, testPort)) {
    client.stop();
    return true;
  }

  // Dự phòng kiểm tra 8.8.8.8 port 53 (DNS port)
  if (client.connect(IPAddress(8, 8, 8, 8), 53)) {
    client.stop();
    return true;
  }

  return false;
}

void performWatchdogCheck() {
  Serial.println();
  Serial.println("[WATCHDOG]");

  bool wifiOk = (WiFi.status() == WL_CONNECTED);
  Serial.print("WiFi: ");
  Serial.println(wifiOk ? "CONNECTED" : "DISCONNECTED");

  Serial.println("Checking Internet...");

  bool internetOk = checkInternetConnection();

  if (internetOk) {
    Serial.println("Internet: OK");
    failureCount = 0; // Reset số lần lỗi nếu kết nối thành công
  } else {
    failureCount++;
    Serial.println("Internet check failed!");
    Serial.print("Failure count: ");
    Serial.print(failureCount);
    Serial.print("/");
    Serial.println(MAX_FAILURES);

    if (failureCount >= MAX_FAILURES) {
      Serial.println("================================");
      Serial.println(" INTERNET FAILED 3 TIMES        ");
      Serial.println(" RESTARTING ESP32...            ");
      Serial.println("================================");
      delay(1000);
      ESP.restart();
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);

  Serial.println();
  Serial.println("==========================================");
  Serial.println("   ESP32 - WIFI & INTERNET WATCHDOG       ");
  Serial.println("==========================================");
#if DEMO_MODE
  Serial.println("Mode: DEMO_MODE (Interval: 15s)");
#else
  Serial.println("Mode: REAL_MODE (Interval: 5 minutes)");
#endif
  Serial.println("Hint: Nhap 'fail' tren Serial de mo phong mat Internet.");
  Serial.println("      Nhap 'ok' de khoi phuc lai ket noi binh thuong.");
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
    Serial.println("\nWiFi connection failed! Will check in watchdog loop.");
  }

  // Thực hiện kiểm tra lần đầu ngay sau khi setup
  performWatchdogCheck();
  lastCheckMs = millis();
}

void loop() {
  unsigned long now = millis();

  // Nhận lệnh tương tác qua Serial
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.equalsIgnoreCase("fail")) {
      simulateFailure = true;
      Serial.println("[CMD] Da bat mo phong mat Internet (simulate failure).");
    } else if (cmd.equalsIgnoreCase("ok")) {
      simulateFailure = false;
      Serial.println("[CMD] Da tat mo phong, tro lai ket noi thuc te.");
    }
  }

  // Định kỳ kiểm tra Internet theo interval đã đặt
  if (now - lastCheckMs >= checkIntervalMs) {
    lastCheckMs = now;
    performWatchdogCheck();
  }

  delay(10);
}

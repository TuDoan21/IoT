#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>

/*
 * BÀI 3: QUẢN LÝ THÔNG TIN WIFI THÔNG MINH (CAPTIVE PORTAL / PREFERENCES)
 * Tác giả: Đoàn Tuấn Tú
 *
 * Tính năng:
 * - Lưu/đọc SSID và Password vào Flash bằng Preferences (không hardcode).
 * - Nếu chưa cấu hình: Tự phát Access Point "SmartDevice_AP" (IP 192.168.4.1).
 * - Cung cấp Captive Portal (DNSServer + WebServer) cho phép người dùng cấu hình qua trình duyệt.
 * - Hỗ trợ TEST_MODE cho Wokwi: Sau 10s ở AP mode, tự động cấu hình Wokwi-GUEST để demo chuyển sang STA.
 * - Hỗ trợ nhập lệnh qua Serial: 'reset' để xóa WiFi, 'demo' để nạp WiFi Wokwi-GUEST.
 */

// Bật chế độ TEST_MODE để tự động demo trên Wokwi nếu không có người dùng thao tác web
#define TEST_MODE true

const char* AP_SSID = "SmartDevice_AP";
const char* AP_PASSWORD = ""; // Không đặt mật khẩu cho AP cấu hình
const byte DNS_PORT = 53;

IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

WebServer server(80);
DNSServer dnsServer;
Preferences preferences;

enum DeviceState {
  STATE_CONNECTING_SAVED,
  STATE_AP_MODE,
  STATE_STA_CONNECTED
};

DeviceState currentState = STATE_CONNECTING_SAVED;

unsigned long stateStartTime = 0;
unsigned long lastDotTime = 0;
bool testTriggered = false;

// HTML giao diện cấu hình WiFi
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Smart Device WiFi Setup</title>
  <style>
    body { font-family: Arial, sans-serif; background: #f4f6f9; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; }
    .card { background: white; padding: 25px; border-radius: 10px; box-shadow: 0 4px 10px rgba(0,0,0,0.1); width: 90%; max-width: 360px; }
    h2 { text-align: center; color: #333; margin-top: 0; }
    label { font-weight: bold; font-size: 14px; color: #555; }
    input[type="text"], input[type="password"] { width: 100%; padding: 10px; margin: 8px 0 16px; border: 1px solid #ccc; border-radius: 5px; box-sizing: border-box; }
    input[type="submit"] { width: 100%; background: #007bff; color: white; padding: 12px; border: none; border-radius: 5px; font-size: 16px; cursor: pointer; }
    input[type="submit"]:hover { background: #0056b3; }
  </style>
</head>
<body>
  <div class="card">
    <h2>WiFi Setup</h2>
    <form action="/save" method="POST">
      <label for="ssid">WiFi SSID:</label>
      <input type="text" id="ssid" name="ssid" required placeholder="Nhập tên WiFi">
      <label for="password">Password:</label>
      <input type="password" id="password" name="password" placeholder="Mật khẩu (nếu có)">
      <input type="submit" value="Lưu và Kết nối">
    </form>
  </div>
</body>
</html>
)rawliteral";

void startAccessPoint();
void connectToWiFi(const String& ssid, const String& pass);
void handleRoot();
void handleSave();

void setup() {
  Serial.begin(115200);
  delay(200);

  Serial.println();
  Serial.println("================================");
  Serial.println("       WIFI CONFIGURATION       ");
  Serial.println("================================");

  preferences.begin("wifi-config", false);
  String savedSSID = preferences.getString("ssid", "");
  String savedPass = preferences.getString("pass", "");

  if (savedSSID.length() > 0) {
    Serial.print("Found saved WiFi: ");
    Serial.println(savedSSID);
    Serial.println("Attempting connection...");
    connectToWiFi(savedSSID, savedPass);
    currentState = STATE_CONNECTING_SAVED;
    stateStartTime = millis();
  } else {
    Serial.println("No saved WiFi found.");
    startAccessPoint();
  }
}

void startAccessPoint() {
  currentState = STATE_AP_MODE;
  stateStartTime = millis();
  testTriggered = false;

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(AP_SSID, AP_PASSWORD);

  // Khởi động DNS Server để chuyển hướng mọi domain về IP của ESP32 (Captive Portal)
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);

  server.on("/", HTTP_GET, handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  // Hỗ trợ redirect captive portal các hệ điều hành
  server.on("/generate_204", HTTP_GET, handleRoot); // Android
  server.on("/hotspot-detect.html", HTTP_GET, handleRoot); // iOS / Apple
  server.onNotFound(handleRoot);
  server.begin();

  Serial.println("Starting Access Point...");
  Serial.print("AP Name: ");
  Serial.println(AP_SSID);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
  Serial.println();
  Serial.println("Open browser and configure WiFi.");
  Serial.println("Hint: Nhap 'demo' tren Serial de tu cau hinh WiFi Wokwi-GUEST.");
  Serial.println("================================");
}

void handleRoot() {
  server.send(200, "text/html", INDEX_HTML);
}

void handleSave() {
  String reqSSID = server.arg("ssid");
  String reqPass = server.arg("password");

  server.send(200, "text/html", "<h3>Da luu cau hinh! Dang ket noi...</h3>");
  delay(500);

  // Lưu vào Flash qua Preferences
  preferences.putString("ssid", reqSSID);
  preferences.putString("pass", reqPass);

  Serial.println();
  Serial.println("Received WiFi configuration from Web Portal:");
  Serial.print("SSID: ");
  Serial.println(reqSSID);

  connectToWiFi(reqSSID, reqPass);
}

void connectToWiFi(const String& ssid, const String& pass) {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  WiFi.begin(ssid.c_str(), pass.c_str());
  currentState = STATE_CONNECTING_SAVED;
  stateStartTime = millis();
  lastDotTime = millis();
}

void loop() {
  unsigned long now = millis();

  // Đọc lệnh từ Serial Monitor để thuận tiện test
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.equalsIgnoreCase("reset")) {
      Serial.println("[COMMAND] Clearing saved WiFi from Flash...");
      preferences.clear();
      WiFi.disconnect(true);
      startAccessPoint();
      return;
    } else if (cmd.equalsIgnoreCase("demo")) {
      Serial.println("[COMMAND] Demo: Saving 'Wokwi-GUEST' to Flash...");
      preferences.putString("ssid", "Wokwi-GUEST");
      preferences.putString("pass", "");
      connectToWiFi("Wokwi-GUEST", "");
      return;
    }
  }

  switch (currentState) {
    case STATE_CONNECTING_SAVED: {
      if (WiFi.status() == WL_CONNECTED) {
        currentState = STATE_STA_CONNECTED;
        Serial.println();
        Serial.println("================================");
        Serial.println("       WIFI CONNECTED           ");
        Serial.println("================================");
        Serial.print("SSID: ");
        Serial.println(WiFi.SSID());
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
        Serial.print("RSSI: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
        Serial.println("================================");
        Serial.println("Nhap 'reset' de xoa cau hinh va mo lai AP.");
      } else {
        // In dấu chấm mỗi 500ms
        if (now - lastDotTime >= 500) {
          Serial.print('.');
          lastDotTime = now;
        }

        // Nếu kết nối thử quá 15 giây mà không thành công, mở lại AP
        if (now - stateStartTime >= 15000) {
          Serial.println();
          Serial.println("[ERROR] Failed to connect to saved WiFi. Switching to AP mode...");
          startAccessPoint();
        }
      }
      break;
    }

    case STATE_AP_MODE: {
      dnsServer.processNextRequest();
      server.handleClient();

      // TEST_MODE cho Wokwi: Sau 10 giây ở AP mode mà chưa cấu hình, tự nạp cấu hình Wokwi-GUEST
      #if TEST_MODE
      if (!testTriggered && (now - stateStartTime >= 10000)) {
        testTriggered = true;
        Serial.println();
        Serial.println("[WOKWI TEST_MODE] Tu dong mo phong nhap thong tin 'Wokwi-GUEST'...");
        preferences.putString("ssid", "Wokwi-GUEST");
        preferences.putString("pass", "");
        connectToWiFi("Wokwi-GUEST", "");
      }
      #endif
      break;
    }

    case STATE_STA_CONNECTED: {
      // Đã kết nối thành công, theo dõi nếu rớt mạng
      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[WARN] WiFi lost connection. Reconnecting...");
        String savedSSID = preferences.getString("ssid", "Wokwi-GUEST");
        String savedPass = preferences.getString("pass", "");
        connectToWiFi(savedSSID, savedPass);
      }
      break;
    }
  }

  delay(5);
}

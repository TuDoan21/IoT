/*
  BÀI VỀ NHÀ 3: ESP32 + DHT22 GỬI DỮ LIỆU LÊN WEBHOOK.SITE (WOKWI SIMULATION)
  --------------------------------------------------------------------------
  Mô phỏng phần cứng IoT trên Wokwi:
  - Vi điều khiển: ESP32 DevKit V1
  - Cảm biến: DHT22 (Đo nhiệt độ và độ ẩm)
  - Chân kết nối: DHT22 Data (SDA) nối vào GPIO 15 của ESP32
  - Mạng Wi-Fi: Wokwi-GUEST (mô phỏng Wi-Fi ảo của Wokwi)
  - Giao thức: HTTP POST định dạng application/json
  - Chu kỳ gửi: 10 giây/lần
  --------------------------------------------------------------------------
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "DHT.h"

// =========================================================================
// 1. CẤU HÌNH THIẾT BỊ VÀ CHÂN CẢM BIẾN
// =========================================================================
#define DHTPIN 15          // Chân GPIO15 kết nối với chân SDA của DHT22
#define DHTTYPE DHT22      // Loại cảm biến là DHT22 (AM2302)

DHT dht(DHTPIN, DHTTYPE);

// =========================================================================
// 2. CẤU HÌNH WI-FI (WOKWI-GUEST CHO NỀN TẢNG MÔ PHỎNG WOKWI)
// =========================================================================
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// =========================================================================
// 3. CẤU HÌNH URL MÁY CHỦ (WEBHOOK.SITE)
// =========================================================================
// URL Webhook.site cá nhân của bạn:
const char* serverUrl = "https://webhook.site/7e245820-4dfb-425a-a195-59e69a15b129";

// Mã định danh thiết bị
const char* deviceId = "esp32_dht22_wokwi";

// Khoảng thời gian định kỳ giữa các lần gửi dữ liệu (10.000 ms = 10 giây)
const unsigned long sendInterval = 10000;
unsigned long lastSendTime = 0;


// =========================================================================
// HÀM KẾT NỐI VÀ KIỂM TRA WI-FI
// =========================================================================
void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  Serial.println("\n[Wi-Fi] Đang kết nối tới mạng: " + String(ssid) + " ...");
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 25) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[Wi-Fi] Kết nối thành công!");
    Serial.print("[Wi-Fi] Địa chỉ IP được cấp: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n[Wi-Fi] [CẢNH BÁO] Kết nối thất bại. Sẽ thử lại sau!");
  }
}


// =========================================================================
// HÀM SETUP (CHẠY 1 LẦN KHI KHỞI ĐỘNG)
// =========================================================================
void setup() {
  // Khởi tạo giao tiếp Serial với tốc độ baud 115200
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n=======================================================");
  Serial.println("   ESP32 + DHT22 IOT STATION - LAB 04 (WOKWI SIMULATION) ");
  Serial.println("=======================================================");

  // Khởi động cảm biến DHT22
  Serial.println("[Cảm biến] Khởi tạo cảm biến DHT22 tại chân GPIO 15...");
  dht.begin();

  // Kết nối Wi-Fi
  connectWiFi();
}


// =========================================================================
// HÀM LOOP (VÒNG LẶP CHÍNH)
// =========================================================================
void loop() {
  unsigned long currentMillis = millis();

  // Kiểm tra chu kỳ 10 giây bằng non-blocking timer
  if (currentMillis - lastSendTime >= sendInterval) {
    lastSendTime = currentMillis;

    Serial.println("\n-------------------------------------------------------");
    Serial.println("[Chu kỳ] Bắt đầu đọc dữ liệu cảm biến (Uptime: " + String(currentMillis / 1000) + "s)");

    // Đảm bảo Wi-Fi vẫn đang kết nối
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("[Wi-Fi] Mất kết nối Wi-Fi! Đang thử kết nối lại...");
      connectWiFi();
      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[Bỏ qua] Không thể gửi HTTP vì chưa có Wi-Fi.");
        return;
      }
    }

    // 1. Đọc dữ liệu từ DHT22
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    // 2. Kiểm tra dữ liệu cảm biến có hợp lệ không
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("[CẢNH BÁO LỖI] Không thể đọc dữ liệu từ cảm biến DHT22!");
      Serial.println("               Kiểm tra lại chân cắm hoặc cảm biến trên mô phỏng.");
      Serial.println("[Hủy gửi] Hủy request POST lần này vì dữ liệu không hợp lệ.");
      return;
    }

    Serial.print("[Đọc được] Nhiệt độ: ");
    Serial.print(temperature);
    Serial.print(" °C | Độ ẩm: ");
    Serial.print(humidity);
    Serial.println(" %");

    // 3. Đóng gói dữ liệu thành JSON bằng thư viện ArduinoJson
    // Tương thích cả ArduinoJson v6 và v7
    #if ARDUINOJSON_VERSION_MAJOR >= 7
      JsonDocument doc;
    #else
      StaticJsonDocument<256> doc;
    #endif

    doc["device_id"] = deviceId;
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["uptime_ms"] = currentMillis;

    String jsonPayload;
    serializeJson(doc, jsonPayload);

    Serial.println("[JSON Payload] Dữ liệu chuẩn bị gửi:");
    Serial.println(jsonPayload);


    // 4. Khởi tạo HTTPClient và gửi HTTP POST request
    HTTPClient http;
    Serial.print("[HTTP] Đang gửi POST tới: ");
    Serial.println(serverUrl);

    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    // Gửi request kèm chuỗi JSON
    int httpResponseCode = http.POST(jsonPayload);

    // 5. Kiểm tra kết quả phản hồi từ máy chủ
    if (httpResponseCode > 0) {
      Serial.print("[HTTP] Mã phản hồi (Status Code): ");
      Serial.println(httpResponseCode);

      String responseBody = http.getString();
      Serial.print("[HTTP] Nội dung phản hồi: ");
      if (responseBody.length() > 0) {
        Serial.println(responseBody);
      } else {
        Serial.println("(Server phản hồi thành công nhưng không có body)");
      }

      if (httpResponseCode == 200) {
        Serial.println("=> [KẾT QUẢ] Gửi dữ liệu IoT thành công!");
      }
    } else {
      Serial.print("[HTTP LỖI] Gửi POST thất bại, mã lỗi nội bộ: ");
      Serial.println(httpResponseCode);
      Serial.print("           Lý do: ");
      Serial.println(http.errorToString(httpResponseCode).c_str());
    }

    // Giải phóng tài nguyên kết nối HTTP
    http.end();
  }

  // Delay ngắn để tránh chiếm trọn CPU vi điều khiển
  delay(10);
}

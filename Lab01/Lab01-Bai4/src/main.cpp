#include <Arduino.h>
#include <DHT.h>

// ========================================================
// LAB 01 - BÀI 4: CẢNH BÁO NHIỆT ĐỘ QUÁ NGƯỠNG
// ========================================================

#define DHTPIN  15
#define DHTTYPE DHT22

const int ALERT_LED = 4;

DHT dht(DHTPIN, DHTTYPE);

// Cấu hình mốc thời gian hoạt động (non-blocking)
const unsigned long DHT_READ_INTERVAL = 2000; // Đọc DHT22 mỗi 2 giây
const unsigned long BLINK_INTERVAL    = 300;  // Nhấp nháy LED mỗi 300ms

unsigned long lastDhtReadTime = 0;
unsigned long lastBlinkTime   = 0;

bool isAlertActive = false;
bool ledState = false;

void setup() {
  Serial.begin(115200);

  // Cấu hình chân LED cảnh báo
  pinMode(ALERT_LED, OUTPUT);
  digitalWrite(ALERT_LED, LOW);

  // Khởi động cảm biến DHT22
  dht.begin();

  Serial.println("--- KHOI DONG BAI 4: CANH BAO NHIET DO DHT22 ---");
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. Đọc cảm biến DHT22 định kỳ mỗi 2 giây
  if (currentMillis - lastDhtReadTime >= DHT_READ_INTERVAL) {
    lastDhtReadTime = currentMillis;

    float temperature = dht.readTemperature();
    float humidity    = dht.readHumidity();

    // Xử lý trường hợp đọc lỗi cảm biến
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Loi: Khong the doc du lieu tu cam bien DHT22!");
      isAlertActive = false;
    } else {
      Serial.print("Nhiet do: ");
      Serial.print(temperature, 1);
      Serial.print(" C | Do am: ");
      Serial.print(humidity, 1);
      Serial.println(" %");

      // Kiểm tra ngưỡng cảnh báo nhiệt độ
      if (temperature > 30.0) {
        isAlertActive = true;
        Serial.println("Trang thai: CANH BAO (Nhiet do > 30 C)");
      } else {
        isAlertActive = false;
        Serial.println("Trang thai: BINH THUONG");
      }
    }
  }

  // 2. Điều khiển LED cảnh báo không dùng delay()
  if (isAlertActive) {
    // Nhiệt độ vượt ngưỡng -> Nhấp nháy LED
    if (currentMillis - lastBlinkTime >= BLINK_INTERVAL) {
      lastBlinkTime = currentMillis;
      ledState = !ledState;
      digitalWrite(ALERT_LED, ledState ? HIGH : LOW);
    }
  } else {
    // Nhiệt độ bình thường -> Tắt LED hoàn toàn
    if (ledState) {
      ledState = false;
      digitalWrite(ALERT_LED, LOW);
    }
  }
}
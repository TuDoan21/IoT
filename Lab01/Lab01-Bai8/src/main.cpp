#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ========================================================
// LAB 01 - BÀI 8: HỆ THỐNG MENU ĐƠN GIẢN TRÊN LCD
// ========================================================

#define DHTPIN  15
#define DHTTYPE DHT22

const int BUTTON_PIN = 18;

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

int page = 1; // Trang hiển thị ban đầu (1 -> 2 -> 3 -> 1)

float currentTemp = 0.0;
float currentHumidity = 0.0;
bool sensorValid = false;

// Quản lý chu kỳ đọc DHT22
unsigned long lastDhtTime = 0;
const unsigned long DHT_INTERVAL = 2000; // Đọc mỗi 2 giây

// Quản lý Debounce nút nhấn bằng millis()
bool lastButtonReading = HIGH;
bool buttonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// Hàm vẽ giao diện theo trang sử dụng switch(page)
void renderMenu() {
  char line1[17];
  char line2[17];

  switch (page) {
    case 1:
      snprintf(line1, sizeof(line1), "%-16s", "Trang 1: Nhiet do");
      if (sensorValid) {
        snprintf(line2, sizeof(line2), "%-16s", (String(currentTemp, 1) + " C").c_str());
      } else {
        snprintf(line2, sizeof(line2), "%-16s", "Loi doc DHT22");
      }
      break;

    case 2:
      snprintf(line1, sizeof(line1), "%-16s", "Trang 2: Do am");
      if (sensorValid) {
        snprintf(line2, sizeof(line2), "%-16s", (String(currentHumidity, 1) + " %").c_str());
      } else {
        snprintf(line2, sizeof(line2), "%-16s", "Loi doc DHT22");
      }
      break;

    case 3:
      snprintf(line1, sizeof(line1), "%-16s", "Trang 3:");
      snprintf(line2, sizeof(line2), "%-16s", sensorValid ? "Trang thai: OK" : "Trang thai: LOI");
      break;

    default:
      page = 1;
      snprintf(line1, sizeof(line1), "%-16s", "Trang 1: Nhiet do");
      snprintf(line2, sizeof(line2), "%-16s", "--.- C");
      break;
  }

  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void setup() {
  Serial.begin(115200);

  // Nút nhấn dùng INPUT_PULLUP
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Khởi tạo I2C và LCD
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();

  // Khởi động cảm biến DHT22
  dht.begin();

  Serial.println("--- KHOI DONG BAI 8: HE THONG MENU LCD ---");

  // Đọc dữ liệu khởi đầu
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (!isnan(t) && !isnan(h)) {
    currentTemp = t;
    currentHumidity = h;
    sensorValid = true;
  }

  renderMenu();
  lastDhtTime = millis();
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. Quét nút nhấn và xử lý debounce bằng millis()
  int reading = digitalRead(BUTTON_PIN);
  if (reading != lastButtonReading) {
    lastDebounceTime = currentMillis;
  }

  if (currentMillis - lastDebounceTime > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      // Khi nhấn nút: tăng số trang và quay vòng 1 -> 2 -> 3 -> 1
      if (buttonState == LOW) {
        page++;
        if (page > 3) {
          page = 1;
        }
        Serial.print("Chuyen sang: Trang ");
        Serial.println(page);
        renderMenu();
      }
    }
  }
  lastButtonReading = reading;

  // 2. Cập nhật dữ liệu DHT22 định kỳ mỗi 2 giây (non-blocking)
  if (currentMillis - lastDhtTime >= DHT_INTERVAL) {
    lastDhtTime = currentMillis;

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (isnan(t) || isnan(h)) {
      sensorValid = false;
      Serial.println("Loi: Khong the doc du lieu tu DHT22!");
    } else {
      sensorValid = true;
      currentTemp = t;
      currentHumidity = h;
      Serial.printf("[Cap nhat DHT22] Nhiet do: %.1f C | Do am: %.1f %%\n", currentTemp, currentHumidity);
    }

    // Cập nhật lại số liệu trên trang hiện tại
    renderMenu();
  }
}
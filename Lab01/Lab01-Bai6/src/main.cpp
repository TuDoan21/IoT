#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ========================================================
// LAB 01 - BÀI 6: TRẠM THỜI TIẾT MINI / SMART THERMOSTAT
// ========================================================

#define DHTPIN  15
#define DHTTYPE DHT22

const int BUTTON_PIN = 18;
const int LED_PIN    = 4; // Mô phỏng máy hút ẩm

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Biến lưu trữ giá trị cảm biến
float currentTempC = 0.0;
float currentHumidity = 0.0;
bool sensorValid = false;

// Quản lý chế độ hiển thị Fahrenheit trong 5 giây bằng millis()
bool showFahrenheit = false;
unsigned long fahrenheitStartTime = 0;
const unsigned long FAHRENHEIT_DURATION = 5000; // 5 giây = 5000ms

// Chu kỳ đọc cảm biến
unsigned long lastSensorReadTime = 0;
const unsigned long SENSOR_READ_INTERVAL = 2000; // Đọc mỗi 2 giây

// Quản lý debounce nút nhấn
bool lastButtonReading = HIGH;
bool buttonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// Cập nhật màn hình LCD (luôn đệm đủ 16 ký tự mỗi dòng)
void updateLCD() {
  char line1[17];
  char line2[17];

  if (!sensorValid) {
    snprintf(line1, sizeof(line1), "%-16s", "Cam bien: LOI");
    snprintf(line2, sizeof(line2), "%-16s", "Kiem tra DHT22");
  } else {
    if (showFahrenheit) {
      float tempF = (currentTempC * 1.8) + 32.0;
      snprintf(line1, sizeof(line1), "%-16s", (String("Nhiet do: ") + String(tempF, 1) + "F").c_str());
    } else {
      snprintf(line1, sizeof(line1), "%-16s", (String("Nhiet do: ") + String(currentTempC, 1) + "C").c_str());
    }
    snprintf(line2, sizeof(line2), "%-16s", (String("Do am:    ") + String(currentHumidity, 1) + "%").c_str());
  }

  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

// In thông tin chi tiết ra Serial Monitor
void printSerialReport() {
  Serial.println("----------------------------------------");
  if (sensorValid) {
    if (showFahrenheit) {
      float tempF = (currentTempC * 1.8) + 32.0;
      Serial.printf("Temperature: %.1f F\n", tempF);
    } else {
      Serial.printf("Temperature: %.1f C\n", currentTempC);
    }
    Serial.printf("Humidity: %.1f %%\n", currentHumidity);
  } else {
    Serial.println("Temperature: ERROR");
    Serial.println("Humidity: ERROR");
  }
  Serial.printf("Dehumidifier: %s\n", (currentHumidity > 80.0 && sensorValid) ? "ON" : "OFF");
  Serial.printf("Display: %s\n", showFahrenheit ? "Fahrenheit" : "Celsius");
}

void setup() {
  Serial.begin(115200);

  // Cấu hình chân nút nhấn và LED
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Khởi tạo giao tiếp I2C (SDA=21, SCL=22) và LCD
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Khoi dong...");

  // Khởi động cảm biến DHT22
  dht.begin();

  Serial.println("--- KHOI DONG BAI 6: TRAM THOI TIET MINI ---");

  // Đọc dữ liệu khởi tạo ban đầu
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (!isnan(t) && !isnan(h)) {
    currentTempC = t;
    currentHumidity = h;
    sensorValid = true;
    digitalWrite(LED_PIN, (currentHumidity > 80.0) ? HIGH : LOW);
  }
  updateLCD();
  printSerialReport();
  lastSensorReadTime = millis();
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. Quét nút nhấn với Debounce bằng millis()
  int reading = digitalRead(BUTTON_PIN);
  if (reading != lastButtonReading) {
    lastDebounceTime = currentMillis;
  }

  if (currentMillis - lastDebounceTime > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      // Khi phát hiện nhấn nút (chuyển sang LOW)
      if (buttonState == LOW) {
        showFahrenheit = true;
        fahrenheitStartTime = currentMillis;
        updateLCD();
        printSerialReport();
      }
    }
  }
  lastButtonReading = reading;

  // 2. Chế độ Fahrenheit tự động trở về Celsius sau 5 giây (Không dùng delay)
  if (showFahrenheit && (currentMillis - fahrenheitStartTime >= FAHRENHEIT_DURATION)) {
    showFahrenheit = false;
    updateLCD();
    printSerialReport();
  }

  // 3. Đọc cảm biến DHT22 định kỳ mỗi 2 giây
  if (currentMillis - lastSensorReadTime >= SENSOR_READ_INTERVAL) {
    lastSensorReadTime = currentMillis;

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (isnan(t) || isnan(h)) {
      sensorValid = false;
    } else {
      sensorValid = true;
      currentTempC = t;
      currentHumidity = h;

      // Điều khiển LED máy hút ẩm: Độ ẩm > 80% bật LED, <= 80% tắt LED
      if (currentHumidity > 80.0) {
        digitalWrite(LED_PIN, HIGH);
      } else {
        digitalWrite(LED_PIN, LOW);
      }
    }

    updateLCD();
    printSerialReport();
  }
}
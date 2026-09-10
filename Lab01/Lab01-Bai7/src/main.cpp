#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ========================================================
// LAB 01 - BÀI 7: ĐỒNG HỒ BẤM GIỜ STOPWATCH
// ========================================================

const int BUTTON_A = 18; // Nút Start / Pause
const int BUTTON_B = 19; // Nút Reset (chỉ hoạt động khi STOPPED)

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Trạng thái của đồng hồ bấm giờ
enum StopwatchState {
  STOPPED,
  RUNNING
};

StopwatchState currentState = STOPPED;

unsigned long startTime = 0;
unsigned long elapsedTime = 0;
unsigned long lastDisplayTime = 0;
const unsigned long DISPLAY_REFRESH_INTERVAL = 50; // Cập nhật màn hình mỗi 50ms

// Debounce cho Nút A
bool lastReadingA = HIGH;
bool buttonStateA = HIGH;
unsigned long lastDebounceTimeA = 0;

// Debounce cho Nút B
bool lastReadingB = HIGH;
bool buttonStateB = HIGH;
unsigned long lastDebounceTimeB = 0;

const unsigned long debounceDelay = 50;

// Hàm định dạng và hiển thị thời gian: MM:SS:ms (ví dụ 02:35:428)
void displayTime(unsigned long currentMs) {
  unsigned long ms = currentMs % 1000;
  unsigned long totalSec = currentMs / 1000;
  unsigned long sec = totalSec % 60;
  unsigned long min = totalSec / 60;

  char timeBuffer[17];
  snprintf(timeBuffer, sizeof(timeBuffer), "%02lu:%02lu:%03lu        ", min, sec, ms);

  lcd.setCursor(0, 1);
  lcd.print(timeBuffer);
}

void setup() {
  Serial.begin(115200);

  // Cấu hình 2 nút nhấn với điện trở kéo lên nội bộ
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);

  // Khởi tạo I2C và màn hình LCD
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();

  // Dòng 1 hiển thị tiêu đề
  lcd.setCursor(0, 0);
  lcd.print("STOPWATCH       ");

  // Dòng 2 khởi tạo 00:00:000
  displayTime(0);

  Serial.println("--- KHOI DONG BAI 7: DONG HO STOPWATCH ---");
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. Debounce và xử lý Nút A (Start / Pause)
  int readingA = digitalRead(BUTTON_A);
  if (readingA != lastReadingA) {
    lastDebounceTimeA = currentMillis;
  }
  if (currentMillis - lastDebounceTimeA > debounceDelay) {
    if (readingA != buttonStateA) {
      buttonStateA = readingA;
      if (buttonStateA == LOW) { // Nhấn nút A
        if (currentState == STOPPED) {
          // Bắt đầu đếm tiếp từ mốc đã tích lũy (không bị reset về 0)
          currentState = RUNNING;
          startTime = currentMillis - elapsedTime;
          Serial.println("START");
        } else {
          // Tạm dừng đồng hồ, lưu lại thời gian đã trôi qua
          currentState = STOPPED;
          elapsedTime = currentMillis - startTime;
          Serial.println("PAUSE");
        }
      }
    }
  }
  lastReadingA = readingA;

  // 2. Debounce và xử lý Nút B (Reset)
  int readingB = digitalRead(BUTTON_B);
  if (readingB != lastReadingB) {
    lastDebounceTimeB = currentMillis;
  }
  if (currentMillis - lastDebounceTimeB > debounceDelay) {
    if (readingB != buttonStateB) {
      buttonStateB = readingB;
      if (buttonStateB == LOW) { // Nhấn nút B
        // Chỉ reset khi đồng hồ đang ở trạng thái STOPPED
        if (currentState == STOPPED) {
          elapsedTime = 0;
          displayTime(0);
          Serial.println("RESET");
        }
      }
    }
  }
  lastReadingB = readingB;

  // 3. Tính toán thời gian khi đang chạy
  if (currentState == RUNNING) {
    elapsedTime = currentMillis - startTime;
  }

  // 4. Cập nhật LCD không dùng delay()
  if (currentMillis - lastDisplayTime >= DISPLAY_REFRESH_INTERVAL) {
    lastDisplayTime = currentMillis;
    displayTime(elapsedTime);
  }
}
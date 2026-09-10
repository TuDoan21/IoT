#include <Arduino.h>

// ========================================================
// LAB 01 - BÀI 2: NÚT BẤM ĐIỀU KHIỂN LED TRỰC TIẾP
// ========================================================

const int BUTTON_PIN = 18;
const int LED_PIN    = 4;

// Biến lưu trạng thái trước đó để chỉ in Serial khi có sự thay đổi
int lastButtonState = -1;

void setup() {
  Serial.begin(115200);

  // Cấu hình nút nhấn với điện trở kéo lên nội bộ (INPUT_PULLUP)
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Cấu hình chân LED ở chế độ OUTPUT
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("--- KHOI DONG BAI 2: NUT BAM DIEU KHIEN LED ---");
}

void loop() {
  // Đọc mức logic của nút nhấn
  int currentButtonState = digitalRead(BUTTON_PIN);

  // Do dùng INPUT_PULLUP:
  // LOW  = Đang nhấn nút -> Bật LED
  // HIGH = Đang nhả nút  -> Tắt LED
  if (currentButtonState == LOW) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  // In thông báo ra Serial Monitor khi có sự thay đổi trạng thái
  if (currentButtonState != lastButtonState) {
    lastButtonState = currentButtonState;
    if (currentButtonState == LOW) {
      Serial.println("Button: PRESSED | LED: ON");
    } else {
      Serial.println("Button: RELEASED | LED: OFF");
    }
  }

  delay(10); // Độ trễ nhỏ ổn định việc đọc tín hiệu
}
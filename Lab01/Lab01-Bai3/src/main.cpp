#include <Arduino.h>

// ========================================================
// LAB 01 - BÀI 3: CÔNG TẮC THÔNG MINH (DEBOUNCE VỚI MILLIS)
// ========================================================

const int BUTTON_PIN = 18;
const int LED_PIN    = 4;

bool ledState = false;          // Trạng thái LED (ban đầu tắt)
bool buttonState = HIGH;        // Trạng thái ổn định của nút nhấn
bool lastButtonReading = HIGH;  // Giá trị đọc thô từ lần quét trước

unsigned long lastDebounceTime = 0;     // Mốc thời gian bắt đầu thay đổi tín hiệu
const unsigned long debounceDelay = 50; // Thời gian chống dội 50ms

void setup() {
  Serial.begin(115200);

  // Nút nhấn dùng INPUT_PULLUP (nối vào GND khi nhấn)
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Cấu hình LED ở chế độ OUTPUT, ban đầu tắt
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("--- KHOI DONG BAI 3: CONG TAC THONG MINH ---");
}

void loop() {
  int reading = digitalRead(BUTTON_PIN);

  // Nếu tín hiệu chân thay đổi (do nhấn phím hoặc nhiễu cơ học), reset mốc thời gian
  if (reading != lastButtonReading) {
    lastDebounceTime = millis();
  }

  // Khi tín hiệu đã ổn định trong khoảng thời gian lớn hơn debounceDelay
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Nếu trạng thái ổn định khác với trạng thái trước đó của nút
    if (reading != buttonState) {
      buttonState = reading;

      // Phát hiện sự kiện nhấn nút (chuyển từ HIGH sang LOW)
      if (buttonState == LOW) {
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState ? HIGH : LOW);

        if (ledState) {
          Serial.println("Button pressed -> LED ON");
        } else {
          Serial.println("Button pressed -> LED OFF");
        }
      }
    }
  }

  lastButtonReading = reading;
}
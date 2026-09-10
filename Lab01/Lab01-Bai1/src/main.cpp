#include <Arduino.h>

// ==========================================
// LAB 01 - BÀI 1: MÔ PHỎNG ĐÈN GIAO THÔNG
// ==========================================

// Định nghĩa chân kết nối LED
const int RED_LED    = 25;
const int YELLOW_LED = 26;
const int GREEN_LED  = 27;

void setup() {
  // Khởi tạo giao tiếp Serial với tốc độ 115200 baud
  Serial.begin(115200);

  // Cấu hình các chân điều khiển LED ở chế độ OUTPUT
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  // Ban đầu tắt tất cả các đèn
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);

  Serial.println("--- KHOI DONG HE THONG DEN GIAO THONG ---");
}

void loop() {
  // 1. ĐÈN XANH sáng trong 3 giây
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
  Serial.println("DEN XANH - Di chuyen");
  delay(3000);

  // 2. ĐÈN VÀNG sáng trong 1 giây
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, HIGH);
  digitalWrite(RED_LED, LOW);
  Serial.println("DEN VANG - Chuan bi dung");
  delay(1000);

  // 3. ĐÈN ĐỎ sáng trong 3 giây
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, HIGH);
  Serial.println("DEN DO - Dung lai");
  delay(3000);
}
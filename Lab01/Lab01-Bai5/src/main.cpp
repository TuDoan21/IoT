#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ========================================================
// LAB 01 - BÀI 5: BỘ ĐẾM THỜI GIAN UPTIME TRÊN LCD
// ========================================================

// Khởi tạo LCD 16x2 với địa chỉ I2C 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Lưu số giây lần trước để chỉ làm mới màn hình khi có thay đổi
unsigned long lastSeconds = 0xFFFFFFFF;

void setup() {
  Serial.begin(115200);

  // Khởi tạo bus I2C cho ESP32 (SDA = GPIO21, SCL = GPIO22)
  Wire.begin(21, 22);

  // Khởi động màn hình LCD và bật đèn nền
  lcd.init();
  lcd.backlight();

  // Hiển thị nội dung cố định ở dòng 1
  lcd.setCursor(0, 0);
  lcd.print("Thoi gian chay:");

  Serial.println("--- KHOI DONG BAI 5: BO DEM UPTIME LCD ---");
}

void loop() {
  // Tính số giây hoạt động dựa vào millis()
  unsigned long currentSeconds = millis() / 1000;

  // Chỉ cập nhật màn hình khi số giây tăng lên để tránh chớp giật màn hình
  if (currentSeconds != lastSeconds) {
    lastSeconds = currentSeconds;

    // Định dạng chuỗi hiển thị có độ rộng cố định 16 ký tự để xóa sạch ký tự thừa
    char lineBuffer[17];
    char text[16];
    snprintf(text, sizeof(text), "%lu giay", currentSeconds);
    snprintf(lineBuffer, sizeof(lineBuffer), "%-16s", text);

    lcd.setCursor(0, 1);
    lcd.print(lineBuffer);

    // In thông tin ra Serial Monitor
    Serial.print("Thoi gian chay: ");
    Serial.print(currentSeconds);
    Serial.println(" giay");
  }
}
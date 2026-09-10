#include <Arduino.h>

// ===================== KHAI BÁO CHÂN =====================
// Board: AZ-Delivery ESP32 DevKit V4 (pinout tương thích esp32dev)
#define PWM_LED_PIN     18   // LED điều khiển độ sáng bằng PWM
#define BUTTON_PIN      19   // Nút nhấn (INPUT_PULLUP)
#define POWER_LED_PIN   2    // LED báo nguồn, nhấp nháy độc lập

// ===================== CẤU HÌNH LEDC (PWM) =====================
#define LEDC_CHANNEL     0        // Kênh PWM (0-15)
#define LEDC_FREQ_HZ     5000     // Tần số 5000 Hz
#define LEDC_RESOLUTION  8        // Độ phân giải 8-bit (0-255)

// ===================== CÁC MỨC ĐỘ SÁNG =====================
const uint8_t brightnessLevels[] = { 0, 64, 128, 191, 255 };   // Duty tương ứng 0/25/50/75/100%
const uint8_t brightnessPercent[] = { 0, 25, 50, 75, 100 };
const uint8_t NUM_LEVELS = sizeof(brightnessLevels) / sizeof(brightnessLevels[0]);

uint8_t currentLevelIndex = 0;   // Bắt đầu ở mức 0%

// ===================== BIẾN DEBOUNCE NÚT NHẤN =====================
const unsigned long DEBOUNCE_DELAY_MS = 50;   // Thời gian chống dội phím
bool lastRawButtonState   = HIGH;   // Trạng thái đọc thô lần trước (HIGH = nhả, do INPUT_PULLUP)
bool stableButtonState    = HIGH;   // Trạng thái đã ổn định (sau debounce)
unsigned long lastDebounceTime = 0;

// ===================== BIẾN LED BÁO NGUỒN (millis) =====================
const unsigned long POWER_LED_HALF_CYCLE_MS = 750;  // 750ms sáng + 750ms tắt = chu kỳ 1.5s
unsigned long lastPowerLedToggleTime = 0;
bool powerLedState = false;

// ===================== HÀM ÁP DỤNG ĐỘ SÁNG HIỆN TẠI =====================
void applyBrightness() {
  uint8_t duty = brightnessLevels[currentLevelIndex];
  ledcWrite(LEDC_CHANNEL, duty);

  Serial.printf("Brightness: %u%% (Duty=%u)\n",
                brightnessPercent[currentLevelIndex], duty);
}

// ===================== HÀM XỬ LÝ NÚT NHẤN (DEBOUNCE BẰNG millis) =====================
void handleButton() {
  bool rawState = digitalRead(BUTTON_PIN);

  // Nếu trạng thái thô thay đổi so với lần đọc trước -> reset bộ đếm thời gian debounce
  if (rawState != lastRawButtonState) {
    lastDebounceTime = millis();
  }

  // Nếu trạng thái thô đã giữ ổn định đủ lâu (qua khỏi thời gian debounce)
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY_MS) {
    // Chỉ xử lý khi trạng thái ổn định THỰC SỰ thay đổi
    if (rawState != stableButtonState) {
      stableButtonState = rawState;

      // Phát hiện cạnh lên (nhả nút): LOW (đang nhấn) -> HIGH (đã thả)
      // Theo yêu cầu đề bài: "nhấn VÀ THẢ nút" mới chuyển mức sáng
      if (stableButtonState == HIGH) {
        currentLevelIndex = (currentLevelIndex + 1) % NUM_LEVELS;
        applyBrightness();
      }
    }
  }

  lastRawButtonState = rawState;
}

// ===================== HÀM XỬ LÝ LED BÁO NGUỒN (NHẤP NHÁY BẰNG millis) =====================
void handlePowerLed() {
  unsigned long currentTime = millis();

  if (currentTime - lastPowerLedToggleTime >= POWER_LED_HALF_CYCLE_MS) {
    lastPowerLedToggleTime = currentTime;
    powerLedState = !powerLedState;
    digitalWrite(POWER_LED_PIN, powerLedState ? HIGH : LOW);
  }
}

// ===================== SETUP =====================
void setup() {
  Serial.begin(115200);
  delay(300); // chỉ chờ Serial ổn định lúc khởi động, KHÔNG dùng trong loop()

  // Cấu hình nút nhấn
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Cấu hình LED báo nguồn (digital thường)
  pinMode(POWER_LED_PIN, OUTPUT);
  digitalWrite(POWER_LED_PIN, LOW);

  // Cấu hình LEDC cho LED PWM
  ledcSetup(LEDC_CHANNEL, LEDC_FREQ_HZ, LEDC_RESOLUTION);
  ledcAttachPin(PWM_LED_PIN, LEDC_CHANNEL);
  ledcWrite(LEDC_CHANNEL, brightnessLevels[currentLevelIndex]); // khởi tạo 0%

  Serial.println("ESP32 BOOT OK");
  Serial.printf("Brightness: %u%% (Duty=%u)\n",
                brightnessPercent[currentLevelIndex],
                brightnessLevels[currentLevelIndex]);

  lastPowerLedToggleTime = millis();
}

// ===================== LOOP (KHÔNG dùng delay()) =====================
void loop() {
  handleButton();
  handlePowerLed();
}
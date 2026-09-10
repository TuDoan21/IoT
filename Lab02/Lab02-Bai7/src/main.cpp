#include <Arduino.h>
#include <DHT.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//================= CẤU HÌNH =================
#define DHTPIN      4
#define DHTTYPE     DHT22
#define BUZZER_PIN  15

#define TEMP_THRESHOLD 30.0
#define READ_INTERVAL  2000
#define BUZZER_TIME    200

DHT dht(DHTPIN, DHTTYPE);

//================= BIẾN DÙNG CHUNG =================
volatile bool alarmActive = false;
volatile float temperature = 0;
volatile float humidity = 0;
volatile bool sensorOK = false;

//================= HÀM IN TERMINAL =================
void printTerminal() {
  Serial.println();
  Serial.println("=================================");
  Serial.println(" HE THONG CANH BAO NHIET DO");
  Serial.println("=================================");

  if (sensorOK) {
    Serial.print("Nhiet do : ");
    Serial.print(temperature, 1);
    Serial.println(" C");

    Serial.print("Do am    : ");
    Serial.print(humidity, 1);
    Serial.println(" %");
  } else {
    Serial.println("Cam bien : LOI DOC DHT22");
  }

  Serial.print("Trang thai: ");
  Serial.println(alarmActive ? "CANH BAO" : "BINH THUONG");

  Serial.println("=================================");
  Serial.flush();
}

//================= TASK ĐỌC DHT22 =================
void TaskReadDHT(void *pvParameters) {
  while (true) {

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      sensorOK = false;
      alarmActive = false;
    } else {
      sensorOK = true;
      humidity = h;
      temperature = t;

      alarmActive = (temperature > TEMP_THRESHOLD);
    }

    printTerminal();

    vTaskDelay(pdMS_TO_TICKS(READ_INTERVAL));
  }
}

//================= TASK BUZZER =================
void TaskBuzzer(void *pvParameters) {

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  while (true) {

    if (alarmActive) {
      digitalWrite(BUZZER_PIN, HIGH);
      vTaskDelay(pdMS_TO_TICKS(BUZZER_TIME));

      digitalWrite(BUZZER_PIN, LOW);
      vTaskDelay(pdMS_TO_TICKS(BUZZER_TIME));
    } else {
      digitalWrite(BUZZER_PIN, LOW);
      vTaskDelay(pdMS_TO_TICKS(50));
    }
  }
}

//================= SETUP =================
void setup() {

  Serial.begin(115200);
  delay(500);

  Serial.println();
  Serial.println("ESP32 BOOT OK");
  Serial.println("Khoi dong he thong...");
  Serial.println();

  dht.begin();

  xTaskCreatePinnedToCore(
      TaskReadDHT,
      "TaskReadDHT",
      4096,
      NULL,
      1,
      NULL,
      1);

  xTaskCreatePinnedToCore(
      TaskBuzzer,
      "TaskBuzzer",
      2048,
      NULL,
      2,
      NULL,
      0);
}

//================= LOOP =================
void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
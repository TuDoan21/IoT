#ifndef CONFIG_H
#define CONFIG_H

// Cấu hình Wi-Fi
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// Cấu hình MQTT Broker
#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "esp32-doantuantu-dht22"

// Tiền tố topic cá nhân và topic môi trường
#define TOPIC_PREFIX "lab05/doantuantu"
#define TOPIC_ENV    "lab05/doantuantu/env"

// Cấu hình cảm biến DHT22 (CHÚ Ý: Dùng đúng DHT22, không dùng DHT11)
#define DHT_PIN 4
#define DHT_TYPE DHT22

// Chu kỳ đọc và gửi dữ liệu (10 giây)
#define DHT_READ_INTERVAL_MS 10000

#endif // CONFIG_H

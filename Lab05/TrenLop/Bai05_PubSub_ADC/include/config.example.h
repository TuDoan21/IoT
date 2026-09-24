#ifndef CONFIG_H
#define CONFIG_H

// Cấu hình Wi-Fi
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// Cấu hình MQTT Broker
#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "esp32-doantuantu-bai05"

// Tiền tố topic cá nhân và các topic giao tiếp
#define TOPIC_PREFIX "lab05/doantuantu"
#define TOPIC_DATA   "lab05/doantuantu/data"
#define TOPIC_CMD    "lab05/doantuantu/cmd"

// Chân đọc tín hiệu ADC (chọn GPIO 34 thuộc khối ADC1 để không bị xung đột với Wi-Fi)
#define ADC_PIN 34

// Chu kỳ gửi mặc định ban đầu (mili-giây)
#define DEFAULT_INTERVAL_MS 5000

#endif // CONFIG_H

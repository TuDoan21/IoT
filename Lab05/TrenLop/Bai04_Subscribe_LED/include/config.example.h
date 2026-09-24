#ifndef CONFIG_H
#define CONFIG_H

// Cấu hình Wi-Fi
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// Cấu hình MQTT Broker
#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "esp32-doantuantu-bai04"

// Tiền tố topic cá nhân và topic điều khiển LED
#define TOPIC_PREFIX "lab05/doantuantu"
#define TOPIC_LED_CONTROL "lab05/doantuantu/control/led"

// Cấu hình chân kết nối LED (GPIO 2 thường là LED tích hợp trên ESP32 DevKit V1)
#define LED_PIN 2

#endif // CONFIG_H

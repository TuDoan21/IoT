#ifndef CONFIG_H
#define CONFIG_H

// Cấu hình Wi-Fi
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// Cấu hình MQTT Broker
#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "esp32-doantuantu-bai03"

// Tiền tố topic cá nhân và topic publish
#define TOPIC_PREFIX "lab05/doantuantu"
#define TOPIC_TEMPERATURE "lab05/doantuantu/sensor/temperature"

// Chu kỳ gửi dữ liệu (mili-giây)
#define PUBLISH_INTERVAL_MS 5000

#endif // CONFIG_H

#ifndef CONFIG_H
#define CONFIG_H

// Cấu hình Wi-Fi
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// Cấu hình MQTT Broker
#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "esp32-doantuantu-lwt"

// Tiền tố topic cá nhân và topic trạng thái LWT
#define TOPIC_PREFIX "lab05/doantuantu"
#define TOPIC_STATUS "lab05/doantuantu/status"

// Chuỗi thông điệp LWT và Online
#define LWT_PAYLOAD_OFFLINE "Offline"
#define LWT_PAYLOAD_ONLINE  "Online"

// Cấu hình QoS và Retain cho LWT
#define LWT_QOS 1
#define LWT_RETAIN true

#endif // CONFIG_H

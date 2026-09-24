# Bài 02: Kết Nối Public HiveMQ Broker

## 1. Mục tiêu
- Tạo kết nối TCP và thiết lập phiên làm việc MQTT giữa ESP32 và Public MQTT Broker (`broker.hivemq.com` trên cổng chuẩn `1883`).
- Sử dụng thư viện `PubSubClient` và `WiFiClient`.
- Cấu hình MQTT Client ID duy nhất để tránh bị broker ngắt kết nối do xung đột phiên.
- Bắt và giải mã các mã trạng thái `client.state()` để chẩn đoán lỗi khi kết nối thất bại.
- Duy trì kết nối liên tục bằng `client.loop()` và cơ chế tự kết nối lại không gây nghẽn vòng lặp.

## 2. Phần cứng yêu cầu
- 01 board ESP32 DevKit V1.
- Cáp kết nối máy tính.
- Đường truyền Internet để ESP32 truy cập đến `broker.hivemq.com`.

## 3. Cấu hình phần mềm
1. Tạo file cấu hình từ mẫu:
   ```bash
   cp include/config.example.h include/config.h
   ```
2. Nội dung file `include/config.h`:
   ```cpp
   #define WIFI_SSID "Wokwi-GUEST"          // hoặc tên Wi-Fi thật của bạn
   #define WIFI_PASSWORD ""                 // mật khẩu Wi-Fi
   #define MQTT_BROKER "broker.hivemq.com"
   #define MQTT_PORT 1883
   #define MQTT_CLIENT_ID "esp32-doantuantu-bai02" // Đảm bảo ID này là duy nhất
   ```

## 4. Cách chạy và nạp chương trình
### Trên VS Code:
```bash
pio run -d TrenLop/Bai02_MQTT_Connect
pio run -d TrenLop/Bai02_MQTT_Connect -t upload
pio run -d TrenLop/Bai02_MQTT_Connect -t monitor
```
### Trên Wokwi Simulator:
- Mở file `TrenLop/Bai02_MQTT_Connect/diagram.json` và ấn **F1** -> **Wokwi: Start Simulator**.
- *Lưu ý*: Môi trường mạng mô phỏng Wokwi kết nối qua gateway trung gian `Wokwi-GUEST`.

## 5. Kết quả mong đợi trên Serial Monitor
Khi chạy thành công:
```text
=========================================
 Lab 05 - Bài 02: Kết nối Public HiveMQ Broker
=========================================

[WiFi] Đang kết nối tới: Wokwi-GUEST
...
[WiFi] Đã kết nối! IP: 10.0.1.15
[MQTT] Đang thử kết nối tới broker broker.hivemq.com:1883 với Client ID: esp32-doantuantu-bai02
[MQTT] KẾT NỐI BROKER THÀNH CÔNG!
[MQTT] Trạng thái hiện tại: MQTT_CONNECTED (Đã kết nối thành công)
```

Nếu ngắt kết nối Internet hoặc cấu hình sai:
```text
[MQTT] KẾT NỐI THẤT BẠI! Mã lỗi (state): -2 -> MQTT_CONNECT_FAILED (Không thể khởi tạo kết nối mạng)
```

## 6. Minh chứng cần chụp ảnh báo cáo
1. **Ảnh chụp màn hình Terminal**: Biên dịch thành công với PlatformIO (`SUCCESS`).
2. **Ảnh chụp Serial Monitor kết nối thành công**: Hiển thị rõ broker `broker.hivemq.com:1883`, Client ID duy nhất và thông báo `KẾT NỐI BROKER THÀNH CÔNG!`.
3. **Ảnh chụp Serial Monitor khi thử nghiệm lỗi (tùy chọn)**: Đổi cổng hoặc broker sai để minh họa bắt mã lỗi `state()`.

## 7. Các lỗi thường gặp và cách xử lý
- **Lỗi `state = -2` (MQTT_CONNECT_FAILED)**: ESP32 chưa có mạng Internet hoặc cổng 1883 bị tường lửa (firewall mạng trường/công ty) chặn.
- **Lỗi ngắt kết nối lặp đi lặp lại liên tục (vừa connect xong lại disconnect)**: Có thiết bị khác đang dùng chung `MQTT_CLIENT_ID`. Hãy đổi chuỗi `MQTT_CLIENT_ID` trong `config.h` sang giá trị khác (ví dụ thêm số ngẫu nhiên `esp32-doantuantu-bai02-9988`).

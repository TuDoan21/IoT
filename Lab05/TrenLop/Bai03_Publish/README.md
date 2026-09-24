# Bài 03: Publish Dữ Liệu Lên MQTT

## 1. Mục tiêu
- Lập trình ESP32 gửi (publish) dữ liệu định kỳ lên một topic xác định trên broker MQTT.
- Sử dụng hàm `millis()` để định thời việc gửi dữ liệu (mỗi 5 giây), tuyệt đối không dùng hàm `delay()` gây nghẽn luồng xử lý MQTT.
- Kiểm tra giá trị trả về của `client.publish()` để xác định gói tin đã được gửi đi thành công hay thất bại.
- Sử dụng công cụ máy khách MQTT (MQTT Explorer) để kiểm chứng dữ liệu nhận được theo thời gian thực.
- **Lưu ý**: Dữ liệu gửi đi ở bài này là chuỗi nhiệt độ **mẫu (mock data)**, ví dụ `Temperature: 28.5C`, chưa phải số đo từ cảm biến vật lý.

## 2. Phần cứng yêu cầu
- 01 board ESP32 DevKit V1.
- Cáp kết nối máy tính.

## 3. Cấu hình phần mềm
Tạo file `include/config.h` (từ `include/config.example.h`):
```cpp
#define WIFI_SSID "Wokwi-GUEST"          // hoặc tên Wi-Fi thật của bạn
#define WIFI_PASSWORD ""                 // mật khẩu Wi-Fi
#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "esp32-doantuantu-bai03"
#define TOPIC_PREFIX "lab05/doantuantu"
#define TOPIC_TEMPERATURE "lab05/doantuantu/sensor/temperature"
#define PUBLISH_INTERVAL_MS 5000
```

## 4. Cách chạy và nạp chương trình
### Trên PlatformIO:
```bash
pio run -d TrenLop/Bai03_Publish
pio run -d TrenLop/Bai03_Publish -t upload
pio run -d TrenLop/Bai03_Publish -t monitor
```

## 5. Hướng dẫn sử dụng MQTT Explorer để kiểm tra
1. Mở phần mềm **MQTT Explorer** trên máy tính (tải tại [mqtt-explorer.com](https://mqtt-explorer.com/)).
2. Tạo kết nối mới (Connections):
   - **Host**: `broker.hivemq.com`
   - **Port**: `1883`
   - **Protocol**: `mqtt://`
   - Bấm **CONNECT**.
3. Cấu hình nhận tin (Subscribe):
   - Bấm vào mục **Advanced**.
   - Thêm topic: `lab05/doantuantu/#` (hoặc cụ thể `lab05/doantuantu/sensor/temperature`).
   - Bấm **BACK** và quan sát cây topic ở khung bên trái.
4. Bạn sẽ thấy topic `lab05` -> `doantuantu` -> `sensor` -> `temperature` xuất hiện và cập nhật giá trị mới sau mỗi 5 giây.

## 6. Kết quả mong đợi trên Serial Monitor
```text
=========================================
 Lab 05 - Bài 03: Publish Dữ Liệu Lên MQTT
=========================================

[WiFi] Đang kết nối tới: Wokwi-GUEST
...
[WiFi] Đã kết nối! IP: 10.0.1.15
[MQTT] Đang thử kết nối tới broker broker.hivemq.com... THÀNH CÔNG!
-----------------------------------------
[MQTT Publish] Topic   : lab05/doantuantu/sensor/temperature
[MQTT Publish] Payload : Temperature: 28.5C
[MQTT Publish] Kết quả : GỬI THÀNH CÔNG (SUCCESS)
*(Lưu ý: Dữ liệu trên là chuỗi mẫu giả lập, chưa phải số đo từ cảm biến)*
-----------------------------------------
[MQTT Publish] Topic   : lab05/doantuantu/sensor/temperature
[MQTT Publish] Payload : Temperature: 28.7C
[MQTT Publish] Kết quả : GỬI THÀNH CÔNG (SUCCESS)
*(Lưu ý: Dữ liệu trên là chuỗi mẫu giả lập, chưa phải số đo từ cảm biến)*
```

## 7. Minh chứng cần chụp ảnh báo cáo
1. **Ảnh chụp màn hình Terminal**: Biên dịch thành công dự án Bài 03.
2. **Ảnh chụp Serial Monitor**: Hiển thị quá trình gửi bản tin định kỳ, in rõ topic, payload `Temperature: 28.5C` và kết quả `SUCCESS`.
3. **Ảnh chụp màn hình MQTT Explorer**: Hiển thị topic `lab05/doantuantu/sensor/temperature` nhận được giá trị tương ứng khớp với thời gian trên Serial Monitor.

## 8. Các lỗi thường gặp và cách xử lý
- **Publish trả về FAILED**: ESP32 đã mất kết nối với broker hoặc kích thước gói tin vượt quá `MQTT_MAX_PACKET_SIZE` mặc định của PubSubClient (mặc định 256 bytes, gói tin của bài chỉ ~30 bytes nên hoàn toàn an toàn).
- **Trên MQTT Explorer không thấy tin**: Kiểm tra xem topic bạn subscribe trên MQTT Explorer có đúng chính xác `lab05/doantuantu/#` hay không (phân biệt chữ hoa/thường).

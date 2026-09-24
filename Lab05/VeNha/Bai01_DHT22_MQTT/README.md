# Bài Về Nhà 01: Trạm Đo Nhiệt Độ & Độ Ẩm DHT22 Qua MQTT

> [!IMPORTANT]
> **LƯU Ý QUAN TRỌNG**: Dự án sử dụng loại cảm biến **DHT22** (AM2302), **KHÔNG** sử dụng DHT11. Toàn bộ cấu hình mã nguồn, sơ đồ chân và thư viện đều được thiết lập chuẩn xác cho DHT22.

## 1. Mục tiêu
- Kết nối cảm biến nhiệt độ & độ ẩm DHT22 với ESP32 qua giao tiếp 1 dây (1-Wire).
- Đọc định kỳ dữ liệu nhiệt độ (°C) và độ ẩm tương đối (%RH) mỗi 10 giây.
- Xử lý lọc dữ liệu lỗi: kiểm tra `isnan()` để phát hiện mất cảm biến hoặc đọc sai; nếu lỗi thì lập tức bỏ qua chu kỳ publish nhằm bảo vệ độ tin cậy của hệ thống.
- Đóng gói dữ liệu cảm biến vào định dạng chuẩn JSON (`{"temp":26.5,"hum":80.0}`) sử dụng thư viện `ArduinoJson`.
- Publish gói tin JSON lên topic `lab05/doantuantu/env` trên broker MQTT.

## 2. Phần cứng yêu cầu và bảng nối chân
### Thiết bị:
- 01 board ESP32 DevKit V1.
- 01 cảm biến DHT22 (dạng module 3 chân có sẵn điện trở kéo lên hoặc cảm biến rời 4 chân).
- 01 điện trở kéo lên 4.7kΩ – 10kΩ (chỉ cần nếu dùng cảm biến DHT22 4 chân rời; nếu dùng module 3 chân thì bỏ qua).
- Dây nối Breadboard.

### Bảng nối chân ESP32 – DHT22:
| Chân Cảm Biến DHT22 | Chân ESP32 | Ghi chú |
| :--- | :--- | :--- |
| **VCC** (Chân 1) | **3V3** | Nguồn 3.3V từ ESP32 |
| **DATA / OUT** (Chân 2) | **GPIO 4** | Đường truyền dữ liệu số (Digital I/O) |
| **NC** (Chân 3 - nếu có) | Không nối | Chân trống |
| **GND** (Chân 4) | **GND** | Nối Mass chung |

## 3. Cấu hình phần mềm
Tạo file `include/config.h` từ `include/config.example.h`:
```cpp
#define WIFI_SSID "Wokwi-GUEST"          // hoặc Wi-Fi thật của bạn
#define WIFI_PASSWORD ""
#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "esp32-doantuantu-dht22"
#define TOPIC_PREFIX "lab05/doantuantu"
#define TOPIC_ENV    "lab05/doantuantu/env"
#define DHT_PIN 4
#define DHT_TYPE DHT22                   // Khai báo chính xác DHT22
#define DHT_READ_INTERVAL_MS 10000       // Chu kỳ 10 giây
```

## 4. Cách chạy và nạp chương trình
### Trên VS Code:
```bash
pio run -d VeNha/Bai01_DHT22_MQTT
pio run -d VeNha/Bai01_DHT22_MQTT -t upload
pio run -d VeNha/Bai01_DHT22_MQTT -t monitor
```
### Trên Wokwi Simulator:
1. Mở file `VeNha/Bai01_DHT22_MQTT/diagram.json`.
2. Bấm **F1** -> **Wokwi: Start Simulator**.
3. Bấm chuột vào cảm biến DHT22 trên sơ đồ trực quan để thay đổi thanh trượt Nhiệt độ và Độ ẩm.

## 5. Hướng dẫn đọc dữ liệu trên MQTT Explorer
1. Kết nối MQTT Explorer đến `broker.hivemq.com:1883`.
2. Thêm topic subscribe: `lab05/doantuantu/env` (hoặc `lab05/doantuantu/#`).
3. Quan sát bản tin cập nhật định kỳ mỗi 10 giây.
4. Trong mục dữ liệu nhận được, bạn sẽ thấy chuỗi JSON hợp lệ:
   ```json
   {"temp":26.5,"hum":80.0}
   ```
5. MQTT Explorer sẽ tự động nhận diện định dạng JSON và cho phép xem biểu đồ (History) hoặc dạng đối tượng cấu trúc.

## 6. Kết quả mong đợi trên Serial Monitor
```text
==================================================
 Lab 05 - Về Nhà 01: Trạm Đo Nhiệt Độ & Độ Ẩm DHT22
==================================================
[DHT22] Cảm biến DHT22 đã được khởi tạo trên chân GPIO 4
[WiFi] Đang kết nối tới: Wokwi-GUEST
...
[WiFi] Đã kết nối! IP: 10.0.1.15
[MQTT] Đang kết nối lại tới broker broker.hivemq.com... THÀNH CÔNG!
--------------------------------------------------
[DHT22 Read]   Nhiệt độ: 26.5 °C | Độ ẩm: 80.0 %
[MQTT Publish] Topic   : lab05/doantuantu/env
[MQTT Publish] Payload : {"temp":26.5,"hum":80.0}
[MQTT Publish] Kết quả : GỬI THÀNH CÔNG (SUCCESS)
```

## 7. Minh chứng cần chụp ảnh báo cáo
1. **Ảnh chụp màn hình Terminal**: Biên dịch thành công dự án với PlatformIO.
2. **Ảnh chụp phần cứng thực tế (hoặc sơ đồ mô phỏng Wokwi)**: Rõ board ESP32 nối với cảm biến DHT22 (chú ý tem/vỏ cảm biến ghi rõ DHT22).
3. **Ảnh chụp Serial Monitor**: Hiển thị rõ giá trị đọc nhiệt độ, độ ẩm, chuỗi JSON được đóng gói chuẩn và dòng kết quả `GỬI THÀNH CÔNG (SUCCESS)`.
4. **Ảnh chụp giao diện MQTT Explorer**: Thấy rõ topic `lab05/doantuantu/env` nhận được gói tin JSON khớp thời gian thực.

## 8. Các lỗi thường gặp và cách xử lý
- **Lỗi `[DHT22 Error] Không thể đọc dữ liệu từ cảm biến DHT22!` (ra giá trị NaN)**:
  - Kiểm tra xem dây tín hiệu có cắm đúng chân GPIO 4 không.
  - Cảm biến DHT22 cần nguồn 3.3V hoặc 5V ổn định. Nếu dùng loại cảm biến trần 4 chân, phải có trở kéo 4.7kΩ–10kΩ giữa chân DATA và chân VCC.
  - Chu kỳ đọc tối thiểu của DHT22 là 2 giây. Không được hạ `DHT_READ_INTERVAL_MS` xuống dưới 2000 ms.
- **Dữ liệu trên MQTT Explorer bị gộp hoặc sai định dạng**: Kiểm tra cấu trúc đóng gói trong `JsonDocument`.

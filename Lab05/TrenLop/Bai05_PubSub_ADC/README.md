# Bài 05: Giao Tiếp Hai Chiều: ADC Và Lệnh Điều Khiển

## 1. Mục tiêu
- Xây dựng mô hình giao tiếp 2 chiều hoàn chỉnh trên một board ESP32: vừa gửi dữ liệu đo (Publish) vừa nhận lệnh điều khiển thời gian thực (Subscribe).
- Đọc giá trị tín hiệu tương tự (Analog) từ chân thuộc khối **ADC1** (cụ thể là **GPIO 34**) để đảm bảo bộ chuyển đổi ADC hoạt động ổn định song song khi module Wi-Fi đang bật (các chân ADC2 sẽ bị vô hiệu hóa khi bật Wi-Fi).
- Publish định kỳ **giá trị ADC thô** (Raw ADC 12-bit, dải giá trị từ 0 đến 4095) lên topic `lab05/doantuantu/data`.
- Lắng nghe lệnh từ topic `lab05/doantuantu/cmd` với các lệnh:
  - `ON`: Cho phép tiếp tục gửi dữ liệu định kỳ.
  - `OFF`: Tạm dừng gửi dữ liệu định kỳ (nhưng ESP32 vẫn giữ kết nối MQTT và tiếp nhận lệnh).
  - `INTERVAL:<ms>`: Thay đổi chu kỳ gửi (ví dụ `INTERVAL:10000` thành 10 giây). Có kiểm tra tính hợp lệ của số và từ chối nếu <= 500 ms hoặc sai định dạng.

## 2. Phần cứng yêu cầu và bảng nối chân
### Thiết bị:
- 01 board ESP32 DevKit V1.
- 01 biến trở xoay (Potentiometer) 10kΩ (hoặc cảm biến quang trở LDR / cảm biến analog).
- Dây cắm breadboard.

### Bảng nối chân:
| Chân Biến Trở | Chân ESP32 | Ghi chú |
| :--- | :--- | :--- |
| **GND** (Chân 1) | **GND** | Nối Mass chung |
| **SIG / Wiper** (Chân giữa) | **GPIO 34** | Tín hiệu Analog ADC1 (Input Only) |
| **VCC** (Chân 3) | **3V3** | Nguồn dương 3.3V |

## 3. Cấu hình phần mềm
Tạo file `include/config.h` từ `include/config.example.h`:
```cpp
#define WIFI_SSID "Wokwi-GUEST"          // hoặc Wi-Fi thật của bạn
#define WIFI_PASSWORD ""
#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "esp32-doantuantu-bai05"
#define TOPIC_PREFIX "lab05/doantuantu"
#define TOPIC_DATA   "lab05/doantuantu/data"
#define TOPIC_CMD    "lab05/doantuantu/cmd"
#define ADC_PIN 34
#define DEFAULT_INTERVAL_MS 5000
```

## 4. Cách chạy và nạp chương trình
### Trên VS Code:
```bash
pio run -d TrenLop/Bai05_PubSub_ADC
pio run -d TrenLop/Bai05_PubSub_ADC -t upload
pio run -d TrenLop/Bai05_PubSub_ADC -t monitor
```
### Trên Wokwi Simulator:
1. Mở file `TrenLop/Bai05_PubSub_ADC/diagram.json`.
2. Bấm **F1** -> **Wokwi: Start Simulator**.
3. Trong giao diện mô phỏng, dùng chuột xoay núm vặn biến trở (`pot1`) từ trái sang phải để thay đổi giá trị điện áp vào chân GPIO 34.

## 5. Hướng dẫn thao tác trên MQTT Explorer
1. Kết nối đến `broker.hivemq.com:1883`.
2. Subscribe topic: `lab05/doantuantu/#`.
3. Quan sát topic `lab05/doantuantu/data` nhận giá trị ADC thô mỗi 5 giây.
4. Gửi lệnh qua khung Publish:
   - Gửi `OFF` đến `lab05/doantuantu/cmd`: ESP32 dừng gửi dữ liệu lên broker.
   - Gửi `ON` đến `lab05/doantuantu/cmd`: ESP32 tiếp tục gửi dữ liệu trở lại.
   - Gửi `INTERVAL:2000` đến `lab05/doantuantu/cmd`: Tần suất gửi tăng lên 2 giây một lần.
   - Gửi `INTERVAL:abc` hoặc `INTERVAL:100`: ESP32 in thông báo từ chối lệnh ra Serial do chu kỳ không hợp lệ.

## 6. Kết quả mong đợi trên Serial Monitor
```text
=========================================
 Lab 05 - Bài 05: Giao Tiếp Hai Chiều (ADC & CMD)
=========================================

[WiFi] Đang kết nối tới: Wokwi-GUEST
...
[WiFi] Đã kết nối! IP: 10.0.1.15
[MQTT] Đang kết nối lại tới broker broker.hivemq.com... THÀNH CÔNG!
[MQTT] Đang subscribe topic lệnh: lab05/doantuantu/cmd
[MQTT] Subscribe topic lệnh THÀNH CÔNG!
-----------------------------------------
[ADC Publish] Topic       : lab05/doantuantu/data
[ADC Publish] Giá trị thô : 2048 (Raw ADC 12-bit: 0 - 4095)
[ADC Publish] Chu kỳ      : 5000 ms
[ADC Publish] Trạng thái  : ĐANG BẬT (ACTIVE)
[ADC Publish] Kết quả     : THÀNH CÔNG (SUCCESS)
-----------------------------------------
[MQTT Callback] Nhận lệnh từ Topic: lab05/doantuantu/cmd
[MQTT Callback] Nội dung lệnh: "OFF"
[Command Response] -> ĐÃ TẠM DỪNG gửi dữ liệu (MQTT vẫn duy trì kết nối).
-----------------------------------------
[ADC Monitor] Trạng thái  : TẠM DỪNG (PAUSED) | Giá trị thô hiện tại: 2048 | MQTT vẫn kết nối và lắng nghe lệnh.
-----------------------------------------
[MQTT Callback] Nhận lệnh từ Topic: lab05/doantuantu/cmd
[MQTT Callback] Nội dung lệnh: "INTERVAL:2000"
[Command Response] -> ĐÃ ĐỔI chu kỳ gửi dữ liệu thành: 2000 ms.
-----------------------------------------
[MQTT Callback] Nhận lệnh từ Topic: lab05/doantuantu/cmd
[MQTT Callback] Nội dung lệnh: "ON"
[Command Response] -> ĐÃ BẬT gửi dữ liệu định kỳ.
-----------------------------------------
[ADC Publish] Topic       : lab05/doantuantu/data
[ADC Publish] Giá trị thô : 3410 (Raw ADC 12-bit: 0 - 4095)
[ADC Publish] Chu kỳ      : 2000 ms
[ADC Publish] Trạng thái  : ĐANG BẬT (ACTIVE)
[ADC Publish] Kết quả     : THÀNH CÔNG (SUCCESS)
```

## 7. Minh chứng cần chụp ảnh báo cáo
1. **Ảnh chụp màn hình Terminal**: PlatformIO build `SUCCESS`.
2. **Ảnh chụp Serial Monitor khi đang gửi dữ liệu (ACTIVE)**: Hiển thị giá trị ADC thô, chu kỳ và `SUCCESS`.
3. **Ảnh chụp Serial Monitor khi nhận lệnh `OFF`**: Hiển thị trạng thái đã tạm dừng nhưng vẫn duy trì kết nối.
4. **Ảnh chụp Serial Monitor khi nhận lệnh `INTERVAL:xxxx`**: Hiển thị chu kỳ mới được cập nhật.
5. **Ảnh chụp giao diện MQTT Explorer**: Cây dữ liệu `data` và khung gửi lệnh `cmd` tương ứng.

## 8. Các lỗi thường gặp và cách xử lý
- **Đọc ADC luôn ra giá trị 0 hoặc 4095 không đổi**:
  - Kiểm tra lại kết nối chân giữa biến trở với GPIO 34.
  - Hãy chắc chắn dùng các chân thuộc **ADC1** (GPIO 32, 33, 34, 35, 36, 39). Nếu dùng chân thuộc **ADC2** (ví dụ GPIO 2, 4, 12, 13, 14, 15, 25, 26, 27), hàm `analogRead()` sẽ bị lỗi hoặc luôn trả về sai khi Wi-Fi đang hoạt động.
- **Sau lệnh `OFF`, gửi `ON` không phản hồi**: Thường do người lập trình bỏ hàm `mqttClient.loop()` khi trạng thái là OFF. Code của bài luôn duy trì gọi `mqttClient.loop()` trong mọi trường hợp.

# Bài 04: Subscribe Và Điều Khiển LED

## 1. Mục tiêu
- Lập trình ESP32 đăng ký nhận dữ liệu (Subscribe) từ topic MQTT `lab05/doantuantu/control/led`.
- Xây dựng hàm `callback(char* topic, byte* payload, unsigned int length)` xử lý an toàn theo `length`, không giả định payload có ký tự kết thúc `\0`.
- Điều khiển bật LED khi nhận chuỗi `"ON"`, tắt LED khi nhận `"OFF"`.
- Bắt lỗi khi nhận lệnh không hợp lệ, giữ nguyên trạng thái LED và in cảnh báo ra Serial.
- Đảm bảo cơ chế tự động **Subscribe lại** topic sau mỗi lần MQTT kết nối lại.

## 2. Phần cứng yêu cầu và bảng nối chân
### Thiết bị:
- 01 board ESP32 DevKit V1.
- 01 đèn LED (đỏ/xanh) và 01 điện trở 220Ω – 330Ω (nếu đấu LED ngoài).
- Dây nối Breadboard.

### Bảng nối chân (áp dụng khi đấu LED ngoài):
| Chân ESP32 | Linh kiện | Ghi chú |
| :--- | :--- | :--- |
| **GPIO 2** | Cực dương LED (Anode - chân dài) | Qua điện trở 220Ω |
| **GND** | Cực âm LED (Cathode - chân ngắn) | Nối thẳng về GND |

> **Lưu ý quan trọng về LED tích hợp**:
> - Đa số board ESP32 DevKit V1 có sẵn một LED màu xanh dương tích hợp gắn với chân **GPIO 2**. Bạn có thể quan sát trực tiếp LED này mà không cần đấu thêm linh kiện ngoài.
> - Nếu board thực tế của bạn không có LED tích hợp ở GPIO 2 hoặc LED bị hỏng, hãy đấu LED rời theo bảng nối chân trên hoặc thay đổi giá trị `#define LED_PIN` trong file `include/config.h` sang chân GPIO khả dụng khác (ví dụ GPIO 4).

## 3. Cấu hình phần mềm
Tạo file `include/config.h` từ `include/config.example.h`:
```cpp
#define WIFI_SSID "Wokwi-GUEST"          // hoặc Wi-Fi của bạn
#define WIFI_PASSWORD ""
#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "esp32-doantuantu-bai04"
#define TOPIC_PREFIX "lab05/doantuantu"
#define TOPIC_LED_CONTROL "lab05/doantuantu/control/led"
#define LED_PIN 2
```

## 4. Cách chạy và nạp chương trình
### Trên VS Code PlatformIO:
```bash
pio run -d TrenLop/Bai04_Subscribe_LED
pio run -d TrenLop/Bai04_Subscribe_LED -t upload
pio run -d TrenLop/Bai04_Subscribe_LED -t monitor
```
### Trên Wokwi:
- Mở `TrenLop/Bai04_Subscribe_LED/diagram.json` và ấn **F1** -> **Wokwi: Start Simulator**. Sơ đồ đã nối sẵn LED màu đỏ và trở 220Ω vào GPIO 2.

## 5. Hướng dẫn điều khiển bằng MQTT Explorer
1. Kết nối MQTT Explorer đến `broker.hivemq.com:1883`.
2. Khung bên phải (**Publish**):
   - **Topic**: `lab05/doantuantu/control/led`
   - **Data format**: Chọn `raw`
   - Để bật LED: Nhập `ON` -> Bấm nút **Publish**.
   - Để tắt LED: Nhập `OFF` -> Bấm nút **Publish**.
   - Để thử nghiệm lỗi: Nhập lệnh khác như `TOGGLE` hoặc `123` -> Bấm **Publish**.

## 6. Kết quả mong đợi trên Serial Monitor
```text
=========================================
 Lab 05 - Bài 04: Subscribe và Điều Khiển LED
=========================================

[WiFi] Đang kết nối tới: Wokwi-GUEST
...
[WiFi] Đã kết nối! IP: 10.0.1.15
[MQTT] Đang kết nối lại tới broker broker.hivemq.com... THÀNH CÔNG!
[MQTT] Đang subscribe topic: lab05/doantuantu/control/led
[MQTT] Subscribe topic thành công! Sẵn sàng nhận lệnh ON/OFF.
-----------------------------------------
[MQTT Callback] Nhận bản tin từ Topic: lab05/doantuantu/control/led
[MQTT Callback] Lệnh nhận được: "ON"
[LED Control] -> Đã BẬT LED (GPIO 2 = HIGH)
-----------------------------------------
[MQTT Callback] Nhận bản tin từ Topic: lab05/doantuantu/control/led
[MQTT Callback] Lệnh nhận được: "OFF"
[LED Control] -> Đã TẮT LED (GPIO 2 = LOW)
-----------------------------------------
[MQTT Callback] Nhận bản tin từ Topic: lab05/doantuantu/control/led
[MQTT Callback] Lệnh nhận được: "TEST"
[LED Control] CẢNH BÁO: Lệnh không hợp lệ! Giữ nguyên trạng thái LED (OFF). Các lệnh hợp lệ là "ON" hoặc "OFF".
```

## 7. Minh chứng cần chụp ảnh báo cáo
1. **Ảnh chụp màn hình Terminal**: Biên dịch thành công với PlatformIO.
2. **Ảnh chụp Serial Monitor và Đèn LED khi nhận lệnh "ON"**: Serial ghi nhận lệnh ON, đèn LED trên board (hoặc trên breadboard) sáng.
3. **Ảnh chụp Serial Monitor và Đèn LED khi nhận lệnh "OFF"**: Serial ghi nhận lệnh OFF, đèn LED tắt.
4. **Ảnh chụp Serial Monitor khi gửi lệnh sai**: Serial hiển thị cảnh báo lệnh không hợp lệ và trạng thái LED không bị thay đổi ngoài ý muốn.
5. **Ảnh chụp giao diện MQTT Explorer**: Ô publish gửi `ON`/`OFF` khớp với topic của bài.

## 8. Các lỗi thường gặp và cách xử lý
- **Gửi lệnh trên MQTT Explorer nhưng ESP32 không phản hồi**:
  - Kiểm tra xem topic publish trên MQTT Explorer có đúng từng ký tự với topic subscribe trong code không (`lab05/doantuantu/control/led`).
  - Kiểm tra xem `mqttClient.loop()` có đang được gọi liên tục trong `loop()` hay không.
- **ESP32 nhận ký tự lạ sau chuỗi lệnh**: Do code giả định buffer có ký tự null. Code của bài đã dùng `memcpy` và gán `\0` thủ công theo đúng tham số `length` của callback nên loại trừ được lỗi này.

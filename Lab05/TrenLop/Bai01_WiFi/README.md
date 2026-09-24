# Bài 01: Thiết Lập Wi-Fi Cho ESP32

## 1. Mục tiêu
- Cấu hình và kết nối ESP32 vào mạng Wi-Fi (trên phần cứng thực tế hoặc trình mô phỏng Wokwi).
- Hiển thị tiến trình kết nối, trạng thái thành công và địa chỉ IP được cấp phát qua Serial Monitor.
- Triển khai cơ chế tự động phát hiện mất kết nối và kết nối lại (auto-reconnect) định kỳ bằng `millis()` không chặn.
- Bảo mật thông tin bằng cách tách riêng thông tin đăng nhập vào `config.h` và không in mật khẩu ra Serial.

## 2. Phần cứng yêu cầu
- 01 board ESP32 DevKit V1 (30 chân hoặc 36 chân).
- 01 cáp Micro-USB hoặc Type-C kết nối máy tính.
- Mạng Wi-Fi 2.4GHz (ESP32 tiêu chuẩn không hỗ trợ mạng 5GHz thuần).

## 3. Cấu hình phần mềm
1. Trong thư mục `include/`, sao chép file `config.example.h` thành `config.h`:
   ```bash
   cp include/config.example.h include/config.h
   ```
2. Mở file `include/config.h` và chỉnh sửa thông tin mạng:
   - **Mô phỏng Wokwi**: Giữ nguyên `WIFI_SSID = "Wokwi-GUEST"` và `WIFI_PASSWORD = ""`.
   - **Mạch ESP32 thật**: Nhập tên mạng Wi-Fi và mật khẩu của bạn:
     ```cpp
     #define WIFI_SSID "Ten_WiFi_Cua_Ban"
     #define WIFI_PASSWORD "Mat_Khau_WiFi"
     ```

## 4. Cách chạy và nạp chương trình
### Trên VS Code với PlatformIO:
1. Mở thư mục `TrenLop/Bai01_WiFi` bằng VS Code.
2. Mở terminal PlatformIO hoặc dùng thanh công cụ dưới đáy màn hình:
   - **Build**: `pio run`
   - **Upload**: `pio run -t upload`
   - **Serial Monitor**: `pio run -t monitor` (Baudrate: `115200`)

### Trên Wokwi Simulator:
1. Cài đặt tiện ích mở rộng **Wokwi Simulator** trong VS Code.
2. Mở file `diagram.json`.
3. Bấm **F1** và chọn **Wokwi: Start Simulator** để chạy mô phỏng trực tiếp từ file firmware đã build.

## 5. Kết quả mong đợi trên Serial Monitor
Khi khởi động thành công, Serial Monitor sẽ hiển thị như sau:
```text
=========================================
 Lab 05 - Bài 01: Thiết lập Wi-Fi cho ESP32
=========================================

[WiFi] Đang kết nối tới mạng: Wokwi-GUEST
...
[WiFi] Kết nối Wi-Fi thành công!
[WiFi] Địa chỉ IP được cấp: 10.0.1.15
[WiFi] Cường độ tín hiệu (RSSI): -45 dBm
[WiFi] Trạng thái: Đang kết nối | IP: 10.0.1.15
```

### Cách kiểm tra tính năng tự động kết nối lại (Auto-reconnect):
1. **Trên thiết bị thật**: Sau khi ESP32 đã kết nối và in địa chỉ IP, hãy tắt nguồn Router Wi-Fi hoặc tắt tính năng phát điểm truy cập (Mobile Hotspot) trên điện thoại.
2. Quan sát Serial Monitor: Sau khoảng 5 giây, chương trình sẽ phát hiện mất kết nối và in ra:
   ```text
   [WiFi] Cảnh báo: Mất kết nối Wi-Fi! Đang thực hiện kết nối lại...
   ```
3. Bật lại Router Wi-Fi / Hotspot. ESP32 sẽ tự động tái lập kết nối và tiếp tục chu kỳ in trạng thái kèm địa chỉ IP mới/cũ.

## 6. Minh chứng cần chụp ảnh báo cáo
1. **Ảnh chụp màn hình Build thành công**: Terminal hiển thị lệnh PlatformIO build `SUCCESS`.
2. **Ảnh chụp Serial Monitor khi kết nối thành công**: Hiển thị rõ tên SSID, các dấu chấm tiến trình, dòng thông báo kết nối thành công và địa chỉ IP cụ thể.
3. **Ảnh chụp Serial Monitor thử nghiệm Auto-reconnect**: Hiển thị dòng cảnh báo mất Wi-Fi và dòng kết nối lại thành công sau khi bật lại trạm phát.

## 7. Các lỗi thường gặp và cách xử lý
- **ESP32 in ra dấu chấm `.` liên tục không dừng**: 
  - Do sai tên SSID hoặc mật khẩu.
  - Do mạng Wi-Fi phát ở băng tần 5GHz (cần chuyển sang phát băng tần 2.4GHz).
- **Serial Monitor hiển thị ký tự lạ (rác)**:
  - Do chưa cấu hình baudrate đúng. Đảm bảo cấu hình trong `platformio.ini` là `monitor_speed = 115200`.
- **Không tìm thấy file `config.h` khi build**:
  - Hãy kiểm tra xem bạn đã copy từ `config.example.h` sang `config.h` trong thư mục `include/` hay chưa.

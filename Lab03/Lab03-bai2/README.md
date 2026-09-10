# Bài 2 — WiFi Auto-Reconnect

Tác giả sơ đồ: Đoàn Tuấn Tú.

## Chạy trên VS Code

1. Mở thư mục dự án chứa `platformio.ini` bằng **File → Open Folder**.
2. Cài **PlatformIO IDE** và **Wokwi Simulator** nếu chưa có.
3. Nếu Wokwi chưa kích hoạt, nhấn F1 → **Wokwi: Request a new License** và làm theo hướng dẫn đăng nhập/kích hoạt.
4. Chọn **PlatformIO: Build** hoặc mở PlatformIO Core CLI và chạy `pio run -e esp32dev`.
5. Chờ `[SUCCESS]`. Wokwi đọc `.pio/build/esp32dev/firmware.bin` và `firmware.elf` theo `wokwi.toml`.
6. Nhấn F1 → **Wokwi: Start Simulator**. Quan sát Serial Monitor của Wokwi (115200 baud); không cần Upload hoặc cổng COM của thiết bị thật.
7. Khi sửa code, dừng mô phỏng, Build lại rồi Start Simulator để nạp firmware mới.

Nếu terminal PowerShell không nhận `pio`, máy hiện tại có thể dùng:

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" run -e esp32dev
```

## Cơ chế

- Dùng ESP32 Station, SSID `Wokwi-GUEST`, mật khẩu trống, kênh 6.
- Pin platform `espressif32@7.0.1`, đã build với Arduino ESP32 Core 2.0.17 cài trên máy.
- Callback `WiFi.onEvent(WiFiEvent_t, WiFiEventInfo_t)` nhận `ARDUINO_EVENT_*`.
- Callback chạy trên task riêng, chỉ gửi sự kiện vào queue. `loop()` xử lý log, bộ đếm và mốc thời gian để tránh chia sẻ biến không đồng bộ.
- `WiFi.begin()` chỉ gọi trong setup. Khi chưa có IP, `loop()` gọi `WiFi.reconnect()` mỗi 5000 ms, không giới hạn số lần.
- `WiFi.setAutoReconnect(false)` tắt cơ chế reconnect tự động thông thường của framework để ứng dụng quản lý lịch retry. Core 2.0.17 vẫn có một lần thử lại nội bộ cho lỗi kết nối đầu tiên (ngoại trừ ngắt chủ động); `Retry Count` đếm các lần gọi retry của ứng dụng.
- In số retry khi nhận IP rồi reset bộ đếm về 0. Không gọi reset ESP32.
- Không có vòng lặp chờ WiFi, không có delay dài. Queue được đọc không chờ, tối đa 16 sự kiện mỗi vòng. `[STATUS]` xuất hiện mỗi 10 giây.
- Phép trừ thời gian unsigned chịu được rollover của `millis()`.

## Kiểm tra Auto-Reconnect trong Wokwi

Mặc định `#define TEST_DISCONNECT true`. Không cần tìm nút tắt AP: demo dùng `WiFi.disconnect(false, false)` để ngắt kết nối ESP32 khỏi AP mô phỏng nhưng giữ radio và cấu hình WiFi.

1. Khởi động và đợi `STA_START`, `STA_CONNECTED`, `STA_GOT_IP`, `WIFI CONNECTED SUCCESS`, SSID, IP, RSSI. Lần kết nối đầu thường có `Retry Count: 0`.
2. Sau 15 giây kể từ khi nhận IP, thấy `[TEST] Disconnect WiFi once...`.
3. Thấy `STA_DISCONNECTED` kèm reason và khung `WIFI DISCONNECTED`.
4. Khoảng 5 giây sau, thấy `Retry attempt: 1`. Nếu chưa thành công, tiếp tục thấy 2, 3... cách nhau khoảng 5 giây theo timestamp.
5. Khi có IP lại, thấy `WIFI CONNECTED SUCCESS`, IP và `Retry Count: 1` (hoặc lớn hơn nếu cần nhiều lần thử).
6. Các log `[STATUS]` tiếp theo có `Retry counter: 0`, uptime tiếp tục tăng, không xuất hiện lại `[BOOT]`.
7. Chạy thêm ít nhất 30 giây: `[TEST]` không lặp lại. Cờ `testDone` không được reset khi reconnect.

Đây là ngắt chủ động từ phía ESP32; không mô phỏng AP tắt kéo dài hoặc mất Internet phía sau AP. Tắt WiFi máy tính không phải cách đáng tin cậy để tạo sự kiện mất liên kết với AP ảo Wokwi-GUEST.

Muốn quan sát nhiều lần retry thất bại: tạm đổi SSID thành `Wokwi-GUEST-NOT-FOUND`, Build và chạy khoảng 30 giây. Các lần retry phải cách nhau khoảng 5000 ms và `[STATUS]` vẫn xuất hiện. Sau đó trả SSID về `Wokwi-GUEST`, Build lại. Phép thử này kiểm tra mạng không tồn tại lúc khởi động, không chứng minh AP được bật lại trong cùng phiên mô phỏng.

Khi dùng bình thường, đổi `#define TEST_DISCONNECT false`, Build lại. Muốn demo lại, khởi động lại mô phỏng. Trên thiết bị thật cần đổi SSID/mật khẩu phù hợp (và bỏ kênh 6 nếu AP dùng kênh khác); có thể tắt rồi bật AP để kiểm tra mất sóng thực tế.

## Serial dự kiến (minh họa, chưa phải log đo thực tế)

```text
[BOOT] ESP32 - Bai 2: WiFi Auto-Reconnect
Connecting to WiFi: Wokwi-GUEST
[EVENT] STA_START: WiFi Station started.
[EVENT] STA_CONNECTED: Access Point connected, waiting for IP.
[EVENT] STA_GOT_IP

================================
     WIFI CONNECTED SUCCESS
================================
SSID: Wokwi-GUEST
IP Address: 10.13.37.2
RSSI: -40 dBm
Retry Count: 0
================================

[STATUS] Uptime: 10000 ms
WiFi: CONNECTED
IP: 10.13.37.2
RSSI: -40 dBm
Retry counter: 0
[TEST] Disconnect WiFi once after 15 seconds with IP.
[EVENT] STA_DISCONNECTED: reason=8

================================
       WIFI DISCONNECTED
================================
Waiting for reconnect... Retry interval: 5 seconds.
================================

[STATUS] Uptime: 20000 ms
WiFi: DISCONNECTED
Waiting for reconnect...
Retry counter: 0
[20500 ms] Retry WiFi connection...
Retry attempt: 1
[EVENT] STA_CONNECTED: Access Point connected, waiting for IP.
[EVENT] STA_GOT_IP

================================
     WIFI CONNECTED SUCCESS
================================
SSID: Wokwi-GUEST
IP Address: 10.13.37.2
RSSI: -40 dBm
Retry Count: 1
================================

[STATUS] Uptime: 30000 ms
WiFi: CONNECTED
IP: 10.13.37.2
RSSI: -40 dBm
Retry counter: 0
```

IP, RSSI, reason, timestamp và thứ tự xen kẽ STATUS/event có thể khác. Uptime trong ví dụ giả sử kết nối ban đầu mất khoảng 500 ms.

## Kiểm chứng trong phiên làm việc

- PlatformIO build: **SUCCESS**, Arduino ESP32 Core 2.0.17, platform 7.0.1.
- RAM: 43.448 / 327.680 byte; Flash: 726.665 / 1.310.720 byte.
- Đã tạo firmware BIN và ELF đúng đường dẫn cấu hình Wokwi.
- Đã kiểm tra cấu hình sơ đồ chỉ có một ESP32 DevKit và Serial TX/RX.
- Chưa chạy mô phỏng Wokwi thực tế: phiên làm việc không có công cụ điều khiển VS Code/Wokwi, không có `wokwi-cli`. Các bước kiểm tra runtime ở trên cần thực hiện trong Wokwi; chưa khẳng định đã quan sát callback/reconnect thực tế.

## Tài liệu đối chiếu

- [ESP32 WiFi API và lưu ý callback chạy trên task riêng](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html)
- [Wokwi-GUEST và kênh 6](https://docs.wokwi.com/guides/esp32-wifi)
- [Cài đặt, kích hoạt Wokwi cho VS Code](https://docs.wokwi.com/vscode/getting-started)
- [Đường dẫn firmware/ELF trong wokwi.toml](https://docs.wokwi.com/vscode/project-config)

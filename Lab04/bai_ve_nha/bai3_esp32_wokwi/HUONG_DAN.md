# HƯỚNG DẪN THỰC HÀNH BÀI 3 (VỀ NHÀ): MÔ PHỎNG ESP32 + DHT22 TRÊN WOKWI

## 1. Giới thiệu bài thực hành
Bài thực hành này giúp sinh viên lập trình trực tiếp trên vi điều khiển **ESP32**, giao tiếp với cảm biến đo môi trường thực tế **DHT22**, kết nối mạng Wi-Fi và đóng gói dữ liệu thành chuẩn **JSON** để gửi bằng phương thức **HTTP POST** lên máy chủ đám mây (**Webhook.site**).

> **Lưu ý quan trọng về Địa chỉ IP trên Wokwi:**  
> Trình mô phỏng Wokwi chạy trong môi trường đám mây độc lập (sandbox). Do đó, bạn **không thể** cấu hình địa chỉ `http://127.0.0.1:5000` trên Wokwi để gửi về Flask server trên máy tính của bạn (vì `127.0.0.1` trên Wokwi sẽ trỏ về chính máy ảo Wokwi). Vì vậy, bài này sử dụng URL **Webhook.site** để kiểm tra việc gửi nhận dữ liệu một cách trực quan và ổn định nhất.

---

## 2. Sơ đồ đấu nối phần cứng (Wiring Diagram)

| Chân trên DHT22 | Tên chân | Chân kết nối trên ESP32 DevKit V1 | Dây nối (Wire) |
| :--- | :--- | :--- | :--- |
| Chân 1 | **VCC** (Nguồn) | **3V3** (Nguồn 3.3V) | Đỏ (Red) |
| Chân 2 | **SDA** (Dữ liệu) | **GPIO 15** (D15) | Xanh lá (Green) |
| Chân 3 | **NC** (Không dùng)| Để trống | Không nối |
| Chân 4 | **GND** (Nối đất) | **GND** | Đen (Black) |

---

## 3. Các bước nhập và chạy mô phỏng trên Wokwi.com

### Bước 1: Mở dự án ESP32 mới trên Wokwi
1. Mở trình duyệt web và truy cập vào: [https://wokwi.com](https://wokwi.com)
2. Đăng nhập tài khoản Wokwi (khuyến nghị đăng nhập bằng Google hoặc GitHub để lưu dự án).
3. Tại trang chủ, cuộn xuống phần vi điều khiển và chọn **ESP32** -> Chọn **ESP32 (Arduino)**.

### Bước 2: Nạp mã nguồn (`sketch.ino`)
1. Mở file `sketch.ino` trong thư mục `bai_ve_nha/bai3_esp32_wokwi/`.
2. **QUAN TRỌNG:** Tìm đến dòng 36:
   ```cpp
   const char* serverUrl = "https://webhook.site/7e245820-4dfb-425a-a195-59e69a15b129";
   ```
   Biến `serverUrl` đã được cấu hình với URL Webhook.site cá nhân của bạn.
3. Sao chép toàn bộ nội dung file `sketch.ino` và dán đè vào tab code `sketch.ino` trên Wokwi.

### Bước 3: Cài đặt thư viện (`libraries.txt`)
1. Trên giao diện Wokwi, chọn tab **Library Manager** (biểu tượng hình cuốn sách hoặc tab `libraries.txt`).
2. Thêm 3 thư viện sau (hoặc copy từ file `libraries.txt`):
   ```text
   DHT sensor library
   Adafruit Unified Sensor
   ArduinoJson
   ```

### Bước 4: Nạp sơ đồ mạch (`diagram.json`)
1. Trên giao diện Wokwi, chọn tab `diagram.json`.
2. Sao chép toàn bộ nội dung từ file `bai_ve_nha/bai3_esp32_wokwi/diagram.json` và dán vào tab `diagram.json` trên Wokwi.
3. Ngay lập tức, bạn sẽ thấy trên màn hình mô phỏng xuất hiện bo mạch ESP32 và cảm biến DHT22 đã được nối dây hoàn chỉnh và gọn gàng.

### Bước 5: Bắt đầu chạy mô phỏng
1. Nhấn vào nút **Play (Start the simulation)** màu xanh lá cây ở góc trên.
2. Wokwi sẽ tiến hành biên dịch code và khởi động ESP32.
3. **Quan sát Serial Monitor bên dưới:**
   - ESP32 sẽ kết nối vào mạng Wi-Fi `Wokwi-GUEST`.
   - Báo `[Wi-Fi] Kết nối thành công!`.
   - Đọc giá trị nhiệt độ và độ ẩm từ DHT22.
   - Hiển thị chuỗi JSON: `{"device_id":"esp32_dht22_wokwi","temperature":28.5,"humidity":65,"uptime_ms":...}`.
   - Gửi HTTP POST request và nhận phản hồi `HTTP Status Code: 200`.
4. **Tương tác thay đổi thông số cảm biến:**
   - Khi mô phỏng đang chạy, nhấp chuột vào cảm biến DHT22 trên sơ đồ mạch.
   - Một thanh trượt nhiệt độ và độ ẩm sẽ hiện lên.
   - Kéo thanh trượt để thay đổi giá trị (ví dụ: kéo nhiệt độ lên 35°C, độ ẩm lên 80%).
   - Ở chu kỳ gửi kế tiếp (sau 10 giây), quan sát Serial Monitor và Webhook.site sẽ cập nhật giá trị mới này.

---

## 4. Danh sách ảnh chụp màn hình cần làm minh chứng (Minh Chứng)
Để đạt điểm tối đa bài này, bạn cần chụp **03 ảnh**:

1. **Ảnh 1 - Sơ đồ mạch trên Wokwi:**
   - Chụp toàn bộ khu vực mô phỏng hiển thị rõ kết nối giữa ESP32 và DHT22 (chân VCC vào 3V3, GND vào GND, SDA vào GPIO15).
   - Lưu file: `minh_chung/bai3_wokwi_circuit.png`

2. **Ảnh 2 - Màn hình Serial Monitor gửi thành công:**
   - Chụp khu vực Serial Monitor thể hiện rõ các bước: kết nối Wi-Fi thành công, dữ liệu JSON đóng gói, mã phản hồi `Status Code: 200` và thông báo thành công.
   - Lưu file: `minh_chung/bai3_wokwi_serial_monitor.png`

3. **Ảnh 3 - Webhook.site nhận dữ liệu từ ESP32 Wokwi:**
   - Chụp giao diện Webhook.site hiển thị request POST nhận từ thiết bị `esp32_dht22_wokwi` với đúng nhiệt độ và độ ẩm bạn đã kéo trên Wokwi.
   - Lưu file: `minh_chung/bai3_webhook_esp32_result.png`

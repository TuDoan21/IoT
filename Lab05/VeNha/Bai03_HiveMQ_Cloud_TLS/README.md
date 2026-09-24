# Bài Về Nhà 03: Kết Nối HiveMQ Cloud Có Username/Password & Mã Hóa TLS

## 1. Mục tiêu
- Kết nối ESP32 tới một Private Cluster riêng biệt trên nền tảng đám mây **HiveMQ Cloud** có chứng thực danh tính (Username / Password).
- Mã hóa toàn bộ đường truyền dữ liệu qua giao thức **TLS/SSL trên cổng bảo mật 8883** (HiveMQ Cloud không hỗ trợ cổng không mã hóa 1883).
- Sử dụng thư viện `WiFiClientSecure` và nạp chứng chỉ gốc **Root CA (ISRG Root X1 của Let's Encrypt)** bằng hàm `setCACert()`. Tuyệt đối không dùng `setInsecure()` làm mặc định nhằm ngăn chặn tấn công Man-in-the-Middle (MITM).
- Publish bản tin kiểm thử `"ESP32 connected"` lên topic riêng `lab05/doantuantu/cloud/status`.
- Xử lý và in mã lỗi `client.state()` chi tiết khi xác thực thất bại hoặc mất kết nối.
- Thực nghiệm kịch bản gửi sai Username/Password để chứng minh Cluster từ chối kết nối.

## 2. Phần cứng yêu cầu
- 01 board ESP32 DevKit V1.
- Cáp kết nối máy tính.
- Mạng Wi-Fi có kết nối Internet quốc tế để liên lạc với máy chủ Cloud.

## 3. Hướng dẫn tự tạo Cluster và tài khoản trên HiveMQ Cloud
> [!IMPORTANT]
> Bạn cần tự tạo một cụm máy chủ miễn phí trên HiveMQ Cloud. Không sử dụng hoặc bịa thông tin tài khoản không tồn tại.

1. **Đăng ký tài khoản**: Truy cập [https://www.hivemq.com/cloud/](https://www.hivemq.com/cloud/) và bấm **Sign Up Free** (gói Free hỗ trợ tới 100 thiết bị kết nối đồng thời).
2. **Tạo Cluster**:
   - Sau khi đăng nhập, chọn **Create Serverless Cluster**.
   - Chờ khoảng 1–2 phút để hệ thống khởi tạo cụm máy chủ.
3. **Lấy thông tin kết nối**:
   - Tại trang **Overview**, sao chép giá trị **Cluster URL** (dạng `xxxxxx.s1.eu.hivemq.cloud`).
   - Cổng kết nối hiển thị là **Port: 8883 (TLS)**.
4. **Tạo tài khoản thiết bị (Credentials)**:
   - Chuyển sang tab **Access Management**.
   - Nhập **Username** và **Password** mới cho thiết bị ESP32 (ví dụ: Username: `esp32_user`, Password: do bạn đặt).
   - Chọn quyền hạn (Permissions): Cho phép **Publish & Subscribe**.
   - Bấm **Add Credential**.

## 4. Cấu hình phần mềm
1. Tạo file `include/config.h` từ `include/config.example.h`:
   ```bash
   cp include/config.example.h include/config.h
   ```
2. Điền thông tin cụm máy chủ và tài khoản bạn vừa tạo vào `include/config.h`:
   ```cpp
   #define WIFI_SSID "Ten_WiFi_Nha_Ban"
   #define WIFI_PASSWORD "Mat_Khau_WiFi"

   #define MQTT_BROKER "xxxxxx.s1.eu.hivemq.cloud" // Thay bằng Cluster URL của bạn
   #define MQTT_PORT 8883                          // Cổng TLS bảo mật
   #define MQTT_USERNAME "esp32_user"              // Username tạo ở Bước 3
   #define MQTT_PASSWORD "MatKhau_Ban_Vua_Tao"     // Password tạo ở Bước 3
   #define MQTT_CLIENT_ID "esp32-doantuantu-cloud"
   #define TOPIC_PREFIX "lab05/doantuantu"
   #define TOPIC_CLOUD_STATUS "lab05/doantuantu/cloud/status"
   ```
   *(Chứng chỉ `HIVEMQ_ROOT_CA` đã được đính kèm sẵn trong file cấu hình, khớp với CA Let's Encrypt của HiveMQ Cloud).*

## 5. Cách chạy và nạp chương trình
### Trên VS Code:
```bash
pio run -d VeNha/Bai03_HiveMQ_Cloud_TLS
pio run -d VeNha/Bai03_HiveMQ_Cloud_TLS -t upload
pio run -d VeNha/Bai03_HiveMQ_Cloud_TLS -t monitor
```

## 6. Hướng dẫn kiểm nghiệm kết nối và bảo mật
### A. Kiểm tra kết nối thành công:
1. Nạp code và mở Serial Monitor ở baudrate `115200`.
2. ESP32 sẽ bắt tay TLS với HiveMQ Cloud trên cổng 8883, gửi thông tin đăng nhập và in ra:
   ```text
   [TLS/MQTT] KẾT NỐI VÀ XÁC THỰC HIVEMQ CLOUD THÀNH CÔNG!
   [TLS/MQTT Publish] Topic   : lab05/doantuantu/cloud/status
   [TLS/MQTT Publish] Payload : ESP32 connected
   [TLS/MQTT Publish] Kết quả : THÀNH CÔNG (SUCCESS)
   ```
3. Mở tab **Web Client** trên giao diện điều khiển của HiveMQ Cloud (hoặc dùng MQTT Explorer với cổng 8883 + bật TLS), subscribe topic `lab05/doantuantu/#` để xem tin `"ESP32 connected"`.

### B. Kiểm tra xác nhận cluster từ chối khi dùng sai thông tin:
1. Mở file `config.h`, cố tình sửa `MQTT_PASSWORD` thành mật khẩu sai (ví dụ `"sai_mat_khau_123"`).
2. Nạp lại code và quan sát Serial Monitor:
   ```text
   [TLS/MQTT] KẾT NỐI THẤT BẠI! Mã lỗi state = 4 -> MQTT_CONNECT_BAD_CREDENTIALS (Sai Username hoặc Password trên HiveMQ Cloud)
   >>> HƯỚNG DẪN KIỂM TRA CHẨN ĐOÁN LỖI:
    -> LỖI XÁC THỰC: Kiểm tra lại MQTT_USERNAME và MQTT_PASSWORD trong config.h.
   ```
   Điều này chứng minh cluster riêng của bạn đã bảo vệ an toàn và từ chối các client không được phép.

## 7. Minh chứng cần chụp ảnh báo cáo
1. **Ảnh chụp giao diện quản lý HiveMQ Cloud**: Tab Overview hiển thị Cluster URL đang hoạt động và tab Access Management hiển thị danh sách Credentials bạn đã tạo.
2. **Ảnh chụp Terminal PlatformIO**: Biên dịch thành công với thư viện `WiFiClientSecure`.
3. **Ảnh chụp Serial Monitor kết nối thành công**: Hiển thị quá trình nạp chứng chỉ Root CA, kết nối TLS cổng 8883 và bản tin `"ESP32 connected"` thành công (chú ý: mật khẩu đã được bảo mật ẩn đi).
4. **Ảnh chụp Web Client trên HiveMQ Cloud (hoặc MQTT Explorer TLS)**: Nhận được tin publish từ ESP32.
5. **Ảnh chụp Serial Monitor kịch bản kiểm tra lỗi xác thực (Sai password)**: Bắt đúng mã lỗi `state = 4 (MQTT_CONNECT_BAD_CREDENTIALS)`.

## 8. Các lỗi thường gặp và cách xử lý
- **Lỗi `state = -2` (MQTT_CONNECT_FAILED) khi dùng TLS**:
  - Sai Cluster URL (lưu ý không để tiền tố `tls://` hay `https://` ở đầu URL).
  - Chưa mở cổng mạng hoặc Wi-Fi trường/công ty chặn cổng 8883.
  - Bộ nhớ RAM của ESP32 không đủ cho bộ đệm TLS (ESP32 DevKit V1 thông thường có đủ ~45KB RAM trống cho TLS).
- **Lỗi `state = 4` (MQTT_CONNECT_BAD_CREDENTIALS)**: Nhập sai Username hoặc Password trong `config.h` so với thông tin tạo trên HiveMQ Cloud.
- **Trình mô phỏng Wokwi**: Mạng ảo Wokwi có thể bị giới hạn hoặc chặn cổng outbound TLS 8883 tùy theo chính sách proxy; do đó bài này được khuyến nghị chạy trực tiếp trên board ESP32 thật có kết nối Internet.

# BÁO CÁO VÀ HƯỚNG DẪN THỰC HÀNH LAB 05: ESP32, WI-FI VÀ MQTT

> **Học phần**: Thực hành Internet of Things (IoT)  
> **Nền tảng**: ESP32 DevKit V1, Framework Arduino, PlatformIO (VS Code) & Trình mô phỏng Wokwi  
> **Tiền tố Topic cá nhân**: `lab05/doantuantu`

---

## 1. Giới Thiệu Chung

### 1.1. Wi-Fi và ESP32
Module ESP32 tích hợp sẵn vi điều khiển Wi-Fi 2.4GHz 802.11 b/g/n và Bluetooth 4.2 BR/EDR & BLE. Trong các ứng dụng IoT, ESP32 đóng vai trò là một thiết bị đầu cuối (Edge Node / Device) kết nối vào mạng cục bộ qua router không dây và sau đó định tuyến ra môi trường mạng Internet.

### 1.2. Giao thức MQTT (Message Queuing Telemetry Transport)
MQTT là giao thức truyền thông mạng dạng xuất bản/đăng ký (Publish/Subscribe), hoạt động trên nền TCP/IP. Khác với kiến trúc Client-Server truyền thống kiểu HTTP (phải thăm dò vòng lặp - Polling tốn tài nguyên):
- **Broker (Máy chủ trung gian)**: Tiếp nhận các gói tin từ người xuất bản và chuyển tiếp chính xác đến những người đăng ký quan tâm theo từng **Topic (Chủ đề)**.
- **Publisher (Bên phát)**: Thiết bị đọc cảm biến và gửi bản tin kèm dữ liệu lên một Topic.
- **Subscriber (Bên nhận)**: Thiết bị hoặc ứng dụng lắng nghe các thông điệp đến từ một Topic xác định.
- **Topic**: Chuỗi phân cấp theo định dạng cây phân tách bằng dấu gạch chéo `/` (ví dụ: `lab05/doantuantu/sensor/temperature`). Topic ký tự đại diện (Wildcard): `+` (thay thế 1 cấp) và `#` (thay thế tất cả các cấp con bên dưới).

---

## 2. Bảng Tổng Hợp 8 Bài Thực Hành

| STT | Tên bài | Thư mục | Phần cứng | Topic Gửi / Nhận | Kết quả mong đợi |
| :---: | :--- | :--- | :--- | :--- | :--- |
| **TL-01** | Thiết lập Wi-Fi cho ESP32 | `TrenLop/Bai01_WiFi` | ESP32 | *(Không dùng MQTT)* | Kết nối mạng Wi-Fi, in IP lên Serial, tự kết nối lại khi mất sóng. |
| **TL-02** | Kết nối Public HiveMQ Broker | `TrenLop/Bai02_MQTT_Connect` | ESP32 | Broker `broker.hivemq.com:1883` | Tạo phiên MQTT bằng Client ID riêng; in mã lỗi `client.state()` khi rớt mạng. |
| **TL-03** | Publish dữ liệu lên MQTT | `TrenLop/Bai03_Publish` | ESP32 | **Pub**: `lab05/doantuantu/sensor/temperature` | Gửi chuỗi nhiệt độ mẫu định kỳ 5s bằng `millis()`, xem được trên MQTT Explorer. |
| **TL-04** | Subscribe và điều khiển LED | `TrenLop/Bai04_Subscribe_LED` | ESP32, LED, Resistor 220Ω (GPIO 2) | **Sub**: `lab05/doantuantu/control/led` | Nhận `ON` bật LED, `OFF` tắt LED; từ chối lệnh sai; tự re-subscribe sau reconnect. |
| **TL-05** | Giao tiếp 2 chiều: ADC & Lệnh | `TrenLop/Bai05_PubSub_ADC` | ESP32, Biến trở Potentiometer (GPIO 34) | **Pub**: `.../data`<br>**Sub**: `.../cmd` | Gửi giá trị ADC thô mỗi 5s; nhận lệnh `ON`/`OFF`/`INTERVAL:<ms>`, giữ kết nối khi OFF. |
| **VN-01** | Trạm đo nhiệt ẩm DHT22 | `VeNha/Bai01_DHT22_MQTT` | ESP32, Cảm biến **DHT22** (GPIO 4) | **Pub**: `lab05/doantuantu/env` | Đọc DHT22 mỗi 10s, lọc lỗi `isnan()`, đóng gói JSON `{"temp":x,"hum":y}` và publish. |
| **VN-02** | Last Will and Testament (LWT) | `VeNha/Bai02_MQTT_LWT` | ESP32 | **LWT/Pub**: `lab05/doantuantu/status` | Kết nối báo `Online` (retained); khi rút nguồn đột ngột, broker tự phát `Offline`. |
| **VN-03** | Kết nối HiveMQ Cloud (TLS) | `VeNha/Bai03_HiveMQ_Cloud_TLS` | ESP32 | **Pub**: `lab05/doantuantu/cloud/status` | Bắt tay TLS cổng 8883 có Root CA ISRG Root X1, xác thực Username/Password, bảo mật cao. |

---

## 3. Cài Đặt Môi Trường và Mở Project

### 3.1. Cài đặt VS Code và PlatformIO
1. Tải và cài đặt **Visual Studio Code** từ [https://code.visualstudio.com/](https://code.visualstudio.com/).
2. Mở mục **Extensions** (phím tắt `Ctrl+Shift+X`), tìm và cài đặt:
   - **PlatformIO IDE** (bởi PlatformIO)
   - **Wokwi Simulator** (nếu muốn mô phỏng ảo)
   - **C/C++** (bởi Microsoft)

### 3.2. Cách mở từng bài
Mỗi bài trong thư mục `TrenLop/` và `VeNha/` là một dự án PlatformIO hoàn chỉnh và độc lập. Bạn có thể mở dự án theo 2 cách:
- **Cách 1 (Khuyên dùng)**: Mở trực tiếp thư mục của bài đó: Trong VS Code chọn **File** -> **Open Folder...** -> Chọn đến thư mục bài cần làm (ví dụ: `D:\2026\IoT\Bai Tap\IoT\Lab05\TrenLop\Bai01_WiFi`).
- **Cách 2**: Mở toàn bộ thư mục gốc `Lab05` rồi dùng terminal điều hướng vào thư mục con của từng bài.

---

## 4. Quản Lý File Cấu Hình (`config.h` & `config.example.h`)

Để đảm bảo an toàn bảo mật, mật khẩu Wi-Fi và tài khoản đám mây **tuyệt đối không được commit lên Git**.
Mỗi project đã có sẵn file mẫu `include/config.example.h` và file `include/config.h` (được đưa vào `.gitignore`).
- Khi chạy trên **Wokwi**: Giữ nguyên `WIFI_SSID = "Wokwi-GUEST"` và `WIFI_PASSWORD = ""`.
- Khi nạp vào **ESP32 thật**: Mở file `include/config.h` của bài đó, điền SSID và Password mạng Wi-Fi 2.4GHz của bạn.
- Với Bài Về Nhà 03: Điền Cluster URL, Username và Password tài khoản HiveMQ Cloud của bạn vào `config.h`.

---

## 5. HƯỚNG DẪN CHI TIẾT CÁCH CHẠY TỪNG BÀI

### 5.1. Bài 01 Trên Lớp — Thiết Lập Wi-Fi Cho ESP32 (`TrenLop/Bai01_WiFi`)
- **Mục tiêu**: Kết nối ESP32 vào mạng Wi-Fi, in địa chỉ IP được cấp, tự động phát hiện mất mạng và kết nối lại.
- **Cấu hình file `include/config.h`**:
  ```cpp
  #define WIFI_SSID "Wokwi-GUEST"          // hoặc tên Wi-Fi thật của bạn
  #define WIFI_PASSWORD ""                 // mật khẩu Wi-Fi
  ```
- **Cách chạy trên Wokwi Simulator**:
  1. Mở file `TrenLop/Bai01_WiFi/diagram.json` trong VS Code.
  2. Bấm phím **F1** -> Gõ chọn **Wokwi: Start Simulator** (hoặc bấm nút Play màu xanh lá).
- **Cách chạy trên ESP32 thật**:
  1. Cắm cáp USB nối ESP32 với máy tính.
  2. Mở Terminal và chạy lệnh nạp + mở Serial Monitor:
     ```powershell
     & "C:\Users\HP\.platformio\penv\Scripts\platformio.exe" run -d "D:\2026\IoT\Bai Tap\IoT\Lab05\TrenLop\Bai01_WiFi" -t upload -t monitor
     ```
- **Thao tác thử nghiệm & quan sát**:
  - Xem IP được cấp in ra màn hình Serial Monitor (ví dụ `10.0.1.15` hoặc `192.168.1.x`).
  - Thử ngắt Wi-Fi (tắt tạm thời Router/Hotspot), sau 5s ESP32 sẽ in cảnh báo: `[WiFi] Cảnh báo: Mất kết nối Wi-Fi! Đang thực hiện kết nối lại...`. Khi bật Wi-Fi trở lại, ESP32 sẽ tự động kết nối lại thành công.

---

### 5.2. Bài 02 Trên Lớp — Kết Nối Public HiveMQ Broker (`TrenLop/Bai02_MQTT_Connect`)
- **Mục tiêu**: ESP32 thiết lập phiên làm việc MQTT với broker công cộng `broker.hivemq.com:1883`, in mã lỗi `client.state()` khi gặp sự cố, duy trì `client.loop()` và tự kết nối lại không gây chặn (non-blocking).
- **Cấu hình file `include/config.h`**:
  ```cpp
  #define WIFI_SSID "Wokwi-GUEST"
  #define WIFI_PASSWORD ""
  #define MQTT_BROKER "broker.hivemq.com"
  #define MQTT_PORT 1883
  #define MQTT_CLIENT_ID "esp32-doantuantu-bai02"
  ```
- **Cách chạy trên Wokwi Simulator**:
  1. Mở file `TrenLop/Bai02_MQTT_Connect/diagram.json`.
  2. Bấm **F1** -> chọn **Wokwi: Start Simulator**.
- **Cách chạy trên ESP32 thật**:
  ```powershell
  & "C:\Users\HP\.platformio\penv\Scripts\platformio.exe" run -d "D:\2026\IoT\Bai Tap\IoT\Lab05\TrenLop\Bai02_MQTT_Connect" -t upload -t monitor
  ```
- **Thao tác thử nghiệm & quan sát**:
  - Khi thành công, Serial Monitor in:
    ```text
    [MQTT] Đang thử kết nối tới broker broker.hivemq.com:1883 với Client ID: esp32-doantuantu-bai02
    [MQTT] KẾT NỐI BROKER THÀNH CÔNG!
    [MQTT] Trạng thái hiện tại: MQTT_CONNECTED (Đã kết nối thành công)
    ```
  - Thử nghiệm chẩn đoán lỗi: Sửa tạm `MQTT_PORT` thành `9999` hoặc tắt mạng, quan sát Serial in ra mã lỗi `state = -2` kèm câu giải thích chẩn đoán.

---

### 5.3. Bài 03 Trên Lớp — Publish Dữ Liệu Lên MQTT (`TrenLop/Bai03_Publish`)
- **Mục tiêu**: ESP32 định kỳ gửi (publish) một chuỗi nhiệt độ mẫu lên topic `lab05/doantuantu/sensor/temperature` mỗi 5 giây bằng `millis()`.
- **Cấu hình file `include/config.h`**:
  ```cpp
  #define TOPIC_TEMPERATURE "lab05/doantuantu/sensor/temperature"
  #define PUBLISH_INTERVAL_MS 5000
  ```
- **Cách chạy trên Wokwi hoặc ESP32 thật**:
  - *Trên Wokwi*: Mở `TrenLop/Bai03_Publish/diagram.json` -> **F1** -> **Wokwi: Start Simulator**.
  - *Trên ESP32 thật*:
    ```powershell
    & "C:\Users\HP\.platformio\penv\Scripts\platformio.exe" run -d "D:\2026\IoT\Bai Tap\IoT\Lab05\TrenLop\Bai03_Publish" -t upload -t monitor
    ```
- **Thao tác trên MQTT Explorer**:
  1. Kết nối MQTT Explorer đến `broker.hivemq.com:1883`.
  2. Bấm **Advanced**, thêm topic subscribe: `lab05/doantuantu/#`.
  3. Bấm **Back** và quan sát: Cứ mỗi 5 giây, nhánh `lab05/doantuantu/sensor/temperature` sẽ nhận chuỗi dữ liệu mẫu:
     ```text
     Temperature: 28.5C
     ```
  4. Serial Monitor đồng thời in: `[MQTT Publish] Kết quả : GỬI THÀNH CÔNG (SUCCESS)`.

---

### 5.4. Bài 04 Trên Lớp — Subscribe Và Điều Khiển LED (`TrenLop/Bai04_Subscribe_LED`)
- **Mục tiêu**: ESP32 đăng ký nhận tin từ topic `lab05/doantuantu/control/led`. Đọc chính xác payload theo tham số `length`. Nhận `ON` thì bật LED (GPIO 2), nhận `OFF` thì tắt LED. Nhận lệnh sai thì cảnh báo và giữ nguyên trạng thái.
- **Sơ đồ đấu nối phần cứng**:
  - Cực dương LED (Anode) nối qua trở 220Ω vào **GPIO 2**.
  - Cực âm LED (Cathode) nối về **GND**.
  *(Đa số mạch ESP32 DevKit V1 có sẵn đèn LED xanh tích hợp ở GPIO 2).*
- **Cách chạy trên Wokwi hoặc ESP32 thật**:
  - *Trên Wokwi*: Mở `TrenLop/Bai04_Subscribe_LED/diagram.json` (đã có sẵn LED đỏ và trở 220Ω) -> Bấm **F1** -> **Wokwi: Start Simulator**.
  - *Trên ESP32 thật*:
    ```powershell
    & "C:\Users\HP\.platformio\penv\Scripts\platformio.exe" run -d "D:\2026\IoT\Bai Tap\IoT\Lab05\TrenLop\Bai04_Subscribe_LED" -t upload -t monitor
    ```
- **Thao tác trên MQTT Explorer để điều khiển LED**:
  1. Trong khung **Publish** bên phải của MQTT Explorer:
     - **Topic**: `lab05/doantuantu/control/led`
     - **Format**: `raw`
  2. Nhập `ON` -> Bấm **Publish**: Đèn LED trên mạch (hoặc trên mô phỏng Wokwi) sáng lên, Serial in: `[LED Control] -> Đã BẬT LED`.
  3. Nhập `OFF` -> Bấm **Publish**: Đèn LED tắt đi, Serial in: `[LED Control] -> Đã TẮT LED`.
  4. Nhập lệnh lạ như `TEST123` -> Bấm **Publish**: Serial in cảnh báo lệnh không hợp lệ, trạng thái LED không bị thay đổi ngoài ý muốn.

---

### 5.5. Bài 05 Trên Lớp — Giao Tiếp Hai Chiều: ADC Và Lệnh (`TrenLop/Bai05_PubSub_ADC`)
- **Mục tiêu**: ESP32 vừa gửi định kỳ giá trị ADC thô (từ chân **GPIO 34 thuộc ADC1**, dải 0–4095) lên topic `lab05/doantuantu/data`, vừa lắng nghe lệnh từ topic `lab05/doantuantu/cmd`. Hỗ trợ các lệnh: `ON`, `OFF` (tạm dừng gửi nhưng vẫn giữ kết nối), `INTERVAL:<ms>` (thay đổi chu kỳ gửi).
- **Sơ đồ đấu nối biến trở (Potentiometer)**:
  - Chân 1 nối **GND**.
  - Chân giữa (Tín hiệu) nối **GPIO 34**.
  - Chân 3 nối **3V3**.
- **Cách chạy trên Wokwi hoặc ESP32 thật**:
  - *Trên Wokwi*: Mở `TrenLop/Bai05_PubSub_ADC/diagram.json` -> Bấm **F1** -> **Wokwi: Start Simulator**. Dùng chuột xoay núm vặn biến trở `pot1` để đổi giá trị.
  - *Trên ESP32 thật*:
    ```powershell
    & "C:\Users\HP\.platformio\penv\Scripts\platformio.exe" run -d "D:\2026\IoT\Bai Tap\IoT\Lab05\TrenLop\Bai05_PubSub_ADC" -t upload -t monitor
    ```
- **Thao tác trên MQTT Explorer**:
  1. Subscribe topic: `lab05/doantuantu/#`.
  2. Mặc định thấy topic `lab05/doantuantu/data` nhận giá trị số nguyên thô (ví dụ `2048`) mỗi 5 giây.
  3. Trong khung Publish, gửi lệnh tới topic `lab05/doantuantu/cmd`:
     - Gửi `OFF`: ESP32 dừng gửi dữ liệu lên broker, nhưng Serial báo MQTT vẫn duy trì loop.
     - Gửi `INTERVAL:2000`: ESP32 cập nhật chu kỳ gửi thành 2000ms.
     - Gửi `ON`: ESP32 tiếp tục gửi dữ liệu ADC thô lên topic `data` với chu kỳ nhanh 2 giây/lần.
     - Gửi `INTERVAL:100` hoặc `INTERVAL:abc`: ESP32 in thông báo từ chối lệnh ra Serial do chu kỳ không hợp lệ (< 500ms hoặc không phải số).

---

### 5.6. Bài Về Nhà 01 — Trạm Đo Nhiệt Độ & Độ Ẩm DHT22 (`VeNha/Bai01_DHT22_MQTT`)
> [!IMPORTANT]
> Dự án sử dụng loại cảm biến **DHT22** (AM2302), **KHÔNG** sử dụng DHT11.
- **Mục tiêu**: Đọc cảm biến DHT22 mỗi 10 giây trên chân **GPIO 4**. Kiểm tra lọc lỗi `isnan()`. Đóng gói chuỗi JSON hợp lệ: `{"temp":26.5,"hum":80.0}` và publish lên topic `lab05/doantuantu/env`.
- **Bảng nối chân ESP32 – DHT22**:
  - Chân 1 (VCC) -> **3V3**
  - Chân 2 (DATA / OUT) -> **GPIO 4**
  - Chân 3 (NC) -> Không nối
  - Chân 4 (GND) -> **GND**
- **Cách chạy trên Wokwi hoặc ESP32 thật**:
  - *Trên Wokwi*: Mở `VeNha/Bai01_DHT22_MQTT/diagram.json` -> Bấm **F1** -> **Wokwi: Start Simulator**. Nhấp chuột vào cảm biến DHT22 trên sơ đồ để trượt đổi nhiệt độ và độ ẩm.
  - *Trên ESP32 thật*:
    ```powershell
    & "C:\Users\HP\.platformio\penv\Scripts\platformio.exe" run -d "D:\2026\IoT\Bai Tap\IoT\Lab05\VeNha\Bai01_DHT22_MQTT" -t upload -t monitor
    ```
- **Thao tác trên MQTT Explorer**:
  1. Subscribe topic `lab05/doantuantu/env`.
  2. Quan sát gói tin JSON nhận được sau mỗi 10 giây:
     ```json
     {"temp":26.5,"hum":80.0}
     ```
  3. Serial Monitor hiển thị nhiệt độ, độ ẩm và kết quả `GỬI THÀNH CÔNG (SUCCESS)`.

---

### 5.7. Bài Về Nhà 02 — Last Will and Testament (LWT) (`VeNha/Bai02_MQTT_LWT`)
- **Mục tiêu**: Thiết lập thông điệp di chúc LWT (`"Offline"`, `retained = true`, `QoS = 1`) cho topic `lab05/doantuantu/status`. Sau khi kết nối, publish `"Online"` (`retained = true`). Khi thiết bị mất nguồn hoặc đứt mạng đột ngột, Broker sẽ tự động gửi bản tin `"Offline"` tới tất cả các subscriber.
- **Cách chạy trên ESP32 thật**:
  ```powershell
  & "C:\Users\HP\.platformio\penv\Scripts\platformio.exe" run -d "D:\2026\IoT\Bai Tap\IoT\Lab05\VeNha\Bai02_MQTT_LWT" -t upload -t monitor
  ```
- **Thao tác kiểm nghiệm LWT trên MQTT Explorer**:
  1. Mở MQTT Explorer và subscribe topic: `lab05/doantuantu/status`.
  2. Cấp nguồn cho ESP32 chạy: Thấy topic `status` xuất hiện giá trị `"Online"`.
  3. **Thử nghiệm ngắt kết nối đột ngột**: Rút hẳn cáp USB của ESP32 khỏi máy tính.
  4. Đợi khoảng 20 – 30 giây (hết chu kỳ keepalive timeout của broker), giá trị topic `lab05/doantuantu/status` trên MQTT Explorer sẽ tự động chuyển thành `"Offline"`.
  *(Lưu ý: Không thể xuất hiện ngay tức thì vì broker cần chờ hết timeout của giao thức TCP).*

---

### 5.8. Bài Về Nhà 03 — Kết Nối HiveMQ Cloud Có TLS (`VeNha/Bai03_HiveMQ_Cloud_TLS`)
- **Mục tiêu**: Kết nối ESP32 tới một cụm máy chủ Private Cluster trên **HiveMQ Cloud** được mã hóa bằng giao thức **TLS/SSL trên cổng 8883**, xác thực danh tính bằng **Username/Password** và nạp chứng chỉ gốc **Root CA ISRG Root X1** của Let's Encrypt bằng hàm `setCACert()` (tuyệt đối không dùng `setInsecure()` làm mặc định).
- **Các bước chuẩn bị tài khoản HiveMQ Cloud**:
  1. Truy cập [https://www.hivemq.com/cloud/](https://www.hivemq.com/cloud/) và đăng ký tài khoản miễn phí.
  2. Chọn **Create Serverless Cluster** (hỗ trợ miễn phí 100 thiết bị).
  3. Sao chép **Cluster URL** (dạng `xxxxxx.s1.eu.hivemq.cloud`) và cổng **8883**.
  4. Vào tab **Access Management**, tạo 1 tài khoản với Username và Password do bạn tự đặt, cấp quyền Publish & Subscribe.
- **Điền cấu hình vào `VeNha/Bai03_HiveMQ_Cloud_TLS/include/config.h`**:
  ```cpp
  #define WIFI_SSID "Ten_WiFi_Nha_Ban"
  #define WIFI_PASSWORD "Mat_Khau_WiFi"
  #define MQTT_BROKER "xxxxxx.s1.eu.hivemq.cloud" // Thay bằng Cluster URL thật
  #define MQTT_PORT 8883
  #define MQTT_USERNAME "Username_Ban_Vua_Tao"
  #define MQTT_PASSWORD "Password_Ban_Vua_Tao"
  #define MQTT_CLIENT_ID "esp32-doantuantu-cloud"
  ```
- **Cách chạy trên ESP32 thật**:
  ```powershell
  & "C:\Users\HP\.platformio\penv\Scripts\platformio.exe" run -d "D:\2026\IoT\Bai Tap\IoT\Lab05\VeNha\Bai03_HiveMQ_Cloud_TLS" -t upload -t monitor
  ```
- **Thao tác kiểm nghiệm kết nối & bảo mật**:
  1. Khi thông tin đúng, Serial Monitor sẽ hiển thị:
     ```text
     [TLS] Đã nạp chứng chỉ Root CA ISRG Root X1 để xác thực máy chủ Cloud.
     [TLS/MQTT] KẾT NỐI VÀ XÁC THỰC HIVEMQ CLOUD THÀNH CÔNG!
     [TLS/MQTT Publish] Topic   : lab05/doantuantu/cloud/status
     [TLS/MQTT Publish] Payload : ESP32 connected
     [TLS/MQTT Publish] Kết quả : THÀNH CÔNG (SUCCESS)
     ```
  2. Mở tab **Web Client** trên console HiveMQ Cloud, subscribe `lab05/doantuantu/#` để xem tin `"ESP32 connected"`.
  3. **Kiểm tra từ chối khi sai mật khẩu**: Mở `config.h`, cố tình sửa `MQTT_PASSWORD` thành mật khẩu sai. Nạp lại chương trình, Serial Monitor sẽ lập tức bắt lỗi:
     ```text
     [TLS/MQTT] KẾT NỐI THẤT BẠI! Mã lỗi state = 4 -> MQTT_CONNECT_BAD_CREDENTIALS (Sai Username hoặc Password trên HiveMQ Cloud)
     ```
     Điều này chứng minh cụm máy chủ bảo mật của bạn hoạt động chính xác và từ chối các client không được ủy quyền.

---

## 6. Hướng Dẫn Sử Dụng MQTT Explorer

1. Tải công cụ **MQTT Explorer** tại: [https://mqtt-explorer.com/](https://mqtt-explorer.com/).
2. Thiết lập kết nối:
   - **Protocol**: `mqtt://` (với Public Broker) hoặc `mqtts://` (với HiveMQ Cloud).
   - **Host**: `broker.hivemq.com` (hoặc Cluster URL).
   - **Port**: `1883` (hoặc `8883` nếu dùng TLS).
   - Bấm **CONNECT**.
3. **Đăng ký nhận tin (Subscribe)**:
   - Bấm nút **Advanced**.
   - Thêm Topic: `lab05/doantuantu/#`.
   - Bấm **Add** -> Bấm **Back**.
4. **Gửi lệnh (Publish)**:
   - Khung bên phải phần **Publish**:
   - Nhập Topic: ví dụ `lab05/doantuantu/control/led` hoặc `lab05/doantuantu/cmd`.
   - Chọn định dạng: `raw`
   - Nhập nội dung: ví dụ `ON`, `OFF`, `INTERVAL:2000`.
   - Bấm **Publish**.

---

## 7. Bảng Minh Chứng Cần Chụp Cho Từng Bài

| STT | Tên bài | Các màn hình minh chứng bắt buộc cần chụp |
| :---: | :--- | :--- |
| **01** | Thiết lập Wi-Fi | 1. Terminal PlatformIO báo `SUCCESS`.<br>2. Serial Monitor hiển thị IP được cấp.<br>3. Serial Monitor hiển thị cảnh báo mất Wi-Fi và tự kết nối lại khi tắt router. |
| **02** | Kết nối HiveMQ Broker | 1. PlatformIO build `SUCCESS`.<br>2. Serial Monitor báo `KẾT NỐI BROKER THÀNH CÔNG!` kèm Client ID.<br>3. Serial Monitor hiển thị mã lỗi `state` khi ngắt mạng (chẩn đoán lỗi). |
| **03** | Publish dữ liệu | 1. Serial Monitor in topic, payload `Temperature: 28.5C` và kết quả `SUCCESS`.<br>2. Giao diện MQTT Explorer hiển thị topic `lab05/doantuantu/sensor/temperature` nhận dữ liệu mẫu. |
| **04** | Subscribe & LED | 1. MQTT Explorer gửi `ON`, Serial Monitor in `BẬT LED`, đèn LED sáng.<br>2. MQTT Explorer gửi `OFF`, Serial Monitor in `TẮT LED`, đèn LED tắt.<br>3. Gửi lệnh sai (ví dụ `ABC`), Serial in cảnh báo từ chối, LED giữ nguyên. |
| **05** | ADC & Lệnh (2 chiều) | 1. Serial Monitor gửi giá trị ADC thô mỗi 5s.<br>2. MQTT Explorer gửi `OFF`, Serial báo tạm dừng nhưng vẫn giữ kết nối.<br>3. MQTT Explorer gửi `INTERVAL:2000` và `ON`, dữ liệu gửi lại với chu kỳ 2s. |
| **06** | Cảm biến DHT22 | 1. Mạch thật (hoặc Wokwi) có cảm biến DHT22 nối GPIO 4.<br>2. Serial Monitor hiển thị nhiệt độ, độ ẩm và gói tin JSON `{"temp":..,"hum":..}`.<br>3. MQTT Explorer hiển thị chuỗi JSON chuẩn. |
| **07** | MQTT LWT | 1. MQTT Explorer hiển thị topic `.../status` nhận `Online` (retained) khi ESP32 bật.<br>2. Sau khi rút nguồn ESP32 khoảng 20–30s, MQTT Explorer tự đổi sang `Offline`. |
| **08** | HiveMQ Cloud TLS | 1. Giao diện Web Console của HiveMQ Cloud hiển thị Cluster URL và Account.<br>2. Serial Monitor nạp Root CA ISRG Root X1, kết nối cổng 8883 và báo `ESP32 connected`.<br>3. Serial Monitor báo lỗi `state = 4 (BAD_CREDENTIALS)` khi cố tình điền sai pass. |

---

## 8. Bảng Lỗi Thường Gặp và Cách Xử Lý

| Tình huống lỗi | Nguyên nhân | Cách khắc phục |
| :--- | :--- | :--- |
| **ESP32 in dấu `.` mãi không dừng ở Bài 01** | Sai tên/pass Wi-Fi, hoặc đang bắt Wi-Fi 5GHz | Kiểm tra lại SSID/Password; chuyển router phát sóng băng tần 2.4GHz. |
| **Lỗi `client.state() = -2` (CONNECT_FAILED)** | ESP32 chưa có mạng Internet hoặc cổng 1883 bị chặn | Kiểm tra kết nối mạng; thử dùng trạm phát 4G từ điện thoại nếu mạng trường/công ty chặn cổng 1883. |
| **MQTT vừa kết nối xong lại bị ngắt liên tục** | Trùng `MQTT_CLIENT_ID` với thiết bị khác trên cùng Broker | Thay đổi giá trị `MQTT_CLIENT_ID` trong `config.h` thành chuỗi độc nhất ngẫu nhiên. |
| **Gửi lệnh trên MQTT Explorer mà ESP32 không nhận** | Sai topic, sai tiền tố, hoặc chưa subscribe | Kiểm tra chữ hoa/thường trên topic; đảm bảo ESP32 đã gọi hàm `subscribe()` trong code. |
| **Đèn LED ở Bài 04 không sáng dù code báo ON** | Board không có LED tích hợp ở GPIO 2 hoặc cắm ngược cực LED ngoài | Nối LED rời: Anode vào GPIO 2 (qua trở 220Ω), Cathode vào GND; hoặc đổi `#define LED_PIN` sang chân khác trong `config.h`. |
| **Giá trị ADC ở Bài 05 luôn là 0 hoặc 4095** | Đấu sai chân biến trở hoặc dùng nhầm kênh ADC2 | Đảm bảo chân giữa nối vào GPIO 34 (thuộc ADC1). Không dùng chân thuộc ADC2 vì Wi-Fi xung đột với ADC2. |
| **Cảm biến DHT22 đọc ra giá trị `nan`** | Cắm lỏng chân, cấp sai nguồn, hoặc thiếu trở kéo 4.7kΩ–10kΩ | Kiểm tra dây nối GPIO 4, VCC 3.3V/5V, GND; đảm bảo chu kỳ đọc >= 2 giây. |
| **LWT không xuất hiện chữ `Offline` ngay sau khi rút nguồn** | Broker cần chờ hết thời gian Keepalive timeout | Đây là cơ chế chuẩn của MQTT. Cần kiên nhẫn đợi từ 20 – 30 giây để Broker phát hiện đứt kết nối TCP. |
| **HiveMQ Cloud báo lỗi `state = 4`** | Sai Username hoặc Password | Đăng nhập Console HiveMQ Cloud -> Access Management -> Kiểm tra chính xác Username và cấp lại mật khẩu. |
| **HiveMQ Cloud không kết nối được qua Wokwi** | Mạng giả lập Wokwi không hỗ trợ mở cổng TLS 8883 trực tiếp | Khuyến nghị nạp và chạy Bài Về Nhà 03 trực tiếp trên mạch ESP32 thật có kết nối Wi-Fi ra Internet. |

# BÁO CÁO VÀ HƯỚNG DẪN THỰC HÀNH LAB 04 - INTERNET OF THINGS (IoT)
## CHỦ ĐỀ: GIAO THỨC HTTP, RESTFUL API VÀ TRUYỀN DỮ LIỆU CẢM BIẾN IoT

---

## 1. GIỚI THIỆU LAB 4
Trong hệ thống IoT, việc thu thập dữ liệu từ các nút cảm biến (Sensor Nodes / Edge Devices) và truyền tải lên máy chủ đám mây (Cloud Server) hoặc máy chủ phân tích cục bộ là một khâu then chốt. Bài thực hành **Lab 04** trang bị cho sinh viên kiến thức và kỹ năng thực tế về:
- **Giao thức HTTP và phương thức POST**: Khảo sát cấu trúc bản tin HTTP POST (Headers, Request Body, Status Code).
- **Chuẩn định dạng JSON**: Xây dựng gói tin mô tả dữ liệu cảm biến đa dạng, lồng nhau và phát hiện lỗi cú pháp.
- **Lập trình mạng với Python (`requests`)**: Tự động hóa quá trình gửi dữ liệu, gắn kèm Header xác thực (Authentication / API Key / Bearer Token).
- **Lập trình chịu lỗi (Fault-tolerant)**: Kỹ thuật bắt ngoại lệ (Exceptions) và quản lý thời gian chờ (Timeout).
- **Trạm quan trắc định kỳ (Periodic Sender)**: Mô phỏng cảm biến gửi dữ liệu theo chu kỳ thời gian và dừng an toàn với `Ctrl + C`.
- **Phát triển máy chủ REST API cục bộ bằng Flask**: Xây dựng Endpoint nhận POST, kiểm tra dữ liệu và phản hồi mã trạng thái 200/400.
- **Tích hợp phần cứng vi điều khiển ESP32 và DHT22 trên Wokwi**: Lập trình C++/Arduino kết nối Wi-Fi, đọc cảm biến môi trường thực tế và truyền dữ liệu lên Webhook.

---

## 2. CẤU TRÚC THƯ MỤC

Toàn bộ mã nguồn, cấu hình và tài liệu hướng dẫn được tổ chức chuẩn hóa trong thư mục `Lab04`:

```text
Lab04/
├── README.md                           # Tài liệu tổng quan toàn bộ Lab 4
├── requirements.txt                    # Danh sách thư viện Python cần cài đặt
├── .gitignore                          # Cấu hình bỏ qua file nhạy cảm và môi trường ảo
├── config.example.py                   # File mẫu cấu hình Webhook URL và API Key
├── config_loader.py                    # Trình nạp cấu hình thông minh dùng chung
├── postman/                            # TÀI NGUYÊN POSTMAN EXTENSION TRÊN VS CODE
│   ├── Lab04_HTTP_POST.postman_collection.json   # Postman Collection 5 thư mục & Test Scripts
│   ├── Lab04_Webhook.postman_environment.json   # Environment Webhook
│   ├── Lab04_Local.postman_environment.json     # Environment Local Flask Server
│   ├── lab04_requests.http                      # File HTTP chạy nhanh với REST Client
│   └── HUONG_DAN_POSTMAN_VSCODE.md              # Hướng dẫn chi tiết sử dụng Postman trong VS Code
├── bai_tren_lop/                       # 5 BÀI THỰC HÀNH TRÊN LỚP
│   ├── bai1_postman/
│   │   └── HUONG_DAN.md                # Hướng dẫn tạo URL Webhook và gửi POST bằng Postman
│   ├── bai2_json/
│   │   ├── sensor_data.json            # File JSON dữ liệu cảm biến thực tế chuẩn
│   │   └── HUONG_DAN.md                # Hướng dẫn gửi JSON và sửa lỗi cú pháp thường gặp
│   ├── bai3_python_post/
│   │   └── send_sensor.py              # Script Python gửi POST sử dụng thư viện requests
│   ├── bai4_authentication/
│   │   └── send_with_auth.py           # Script Python gửi POST kèm Header xác thực (API Key, Token)
│   └── bai5_timeout_exception/
│       └── send_with_error_handling.py # Script xử lý ngoại lệ và Timeout (3 kịch bản kiểm thử)
├── bai_ve_nha/                         # 3 BÀI THỰC HÀNH VỀ NHÀ
│   ├── bai1_periodic_sender/
│   │   └── periodic_sender.py          # Trạm đo cảm biến gửi định kỳ mỗi 10s, dừng bằng Ctrl+C
│   ├── bai2_flask_server/
│   │   ├── server.py                   # Local Flask Server nhận POST tại /api/sensor
│   │   └── client.py                   # Client kiểm thử gửi dữ liệu tới Flask Server (200 & 400)
│   └── bai3_esp32_wokwi/
│       ├── sketch.ino                  # Mã nguồn C++ nạp cho ESP32 đọc DHT22 gửi HTTP POST
│       ├── diagram.json                # Sơ đồ mạch kết nối ESP32 + DHT22 trên Wokwi
│       ├── libraries.txt               # Danh mục thư viện Arduino cho Wokwi
│       └── HUONG_DAN.md                # Hướng dẫn từng bước mô phỏng và test trên Wokwi.com
└── minh_chung/
    └── README.md                       # Bảng tổng hợp chi tiết 17 ảnh minh chứng cần nộp
```

---

## 3. YÊU CẦU PHẦN MỀM
Trước khi bắt đầu thực hành, máy tính của bạn cần được cài đặt sẵn:
1. **Python 3** (khuyến nghị phiên bản 3.9 trở lên).
2. **pip** (trình quản lý gói thư viện Python đi kèm sẵn với Python).
3. **Postman** (công cụ kiểm thử API - tải tại [postman.com](https://www.postman.com/downloads/)).
4. **Trình duyệt Web** (Google Chrome / Edge / Firefox) để sử dụng [Webhook.site](https://webhook.site) và [Wokwi.com](https://wokwi.com).

---

## 4. HƯỚNG DẪN TẠO MÔI TRƯỜNG ẢO TRÊN WINDOWS
Mở terminal PowerShell hoặc Command Prompt tại thư mục `Lab04` và chạy:

```powershell
# 1. Tạo môi trường ảo có tên .venv
python -m venv .venv

# 2. Kích hoạt môi trường ảo trên Windows PowerShell
.venv\Scripts\activate

# Nếu PowerShell báo lỗi kịch bản thực thi (Execution Policy), chạy lệnh sau rồi kích hoạt lại:
# Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

Khi kích hoạt thành công, bạn sẽ thấy tiền tố `(.venv)` xuất hiện ở đầu dòng lệnh Terminal.

---

## 5. HƯỚNG DẪN CÀI ĐẶT THƯ VIỆN
Sau khi môi trường ảo đã được kích hoạt, tiến hành cài đặt các thư viện cần thiết (`requests`, `Flask`):

```powershell
pip install -r requirements.txt
```

---

## 6. CÁCH CẤU HÌNH URL WEBHOOK.SITE
Các bài 3, 4, 5 (trên lớp) và Bài 1, 3 (về nhà) sử dụng dịch vụ Webhook.site để nhận dữ liệu kiểm thử.

1. Mở trình duyệt web truy cập: [https://webhook.site](https://webhook.site)
2. Sao chép URL hiển thị tại mục **"Your unique URL"** (ví dụ: `https://webhook.site/3f68a2d1-9b1e-450b-9dfc-112233445566`).
3. Tạo file `config.py` từ file mẫu:
   ```powershell
   copy config.example.py config.py
   ```
4. Mở file `config.py` và sửa dòng sau thành URL bạn vừa sao chép:
   ```python
   WEBHOOK_URL = "https://webhook.site/7e245820-4dfb-425a-a195-59e69a15b129"
   ```
*(Lưu ý: File `config.py` đã được đưa vào `.gitignore`, an toàn tuyệt đối không bị lộ khóa).*

---

## 7. LỆNH CHẠY RIÊNG TỪNG BÀI THỰC HÀNH

Đảm bảo bạn đang đứng tại thư mục gốc `Lab04`:

### [Bài 3 trên lớp] Gửi HTTP POST bằng Python requests:
```powershell
python bai_tren_lop/bai3_python_post/send_sensor.py
```

### [Bài 4 trên lớp] Gửi HTTP POST kèm Authentication Headers:
```powershell
python bai_tren_lop/bai4_authentication/send_with_auth.py
```

### [Bài 5 trên lớp] Kiểm tra xử lý Ngoại lệ và Timeout (3 kịch bản):
```powershell
python bai_tren_lop/bai5_timeout_exception/send_with_error_handling.py
```

### [Bài 1 về nhà] Chạy trạm quan trắc gửi định kỳ mỗi 10 giây:
```powershell
python bai_ve_nha/bai1_periodic_sender/periodic_sender.py
```

---

## 8. HƯỚNG DẪN DÙNG POSTMAN EXTENSION TRÊN VS CODE

Nếu bạn đã cài đặt **Postman Extension** trong VS Code, bạn có thể thực hiện toàn bộ các bài tập mà không cần mở app Postman riêng:

1. **Vị trí file cấu hình sẵn:**
   - Collection: [`postman/Lab04_HTTP_POST.postman_collection.json`](postman/Lab04_HTTP_POST.postman_collection.json)
   - Webhook Environment: [`postman/Lab04_Webhook.postman_environment.json`](postman/Lab04_Webhook.postman_environment.json)
   - Local Environment: [`postman/Lab04_Local.postman_environment.json`](postman/Lab04_Local.postman_environment.json)
   - File REST Client (.http): [`postman/lab04_requests.http`](postman/lab04_requests.http)
2. **Cách import nhanh:**
   - Mở biểu tượng **Postman** trên thanh Activity Bar bên trái của VS Code.
   - Nhấn **Import** ở mục **Collections** và chọn file `Lab04_HTTP_POST.postman_collection.json`.
   - Nhấn **Import** ở mục **Environments** và chọn 2 file `Lab04_Webhook.postman_environment.json` và `Lab04_Local.postman_environment.json`.
3. **Thực thi:**
   - Chọn Environment tương ứng (`Lab04 Webhook` cho các bài 1-4, `Lab04 Local` cho bài 5 Flask).
   - Mở từng request và nhấn nút **Send**. Toàn bộ Test Scripts tự động kiểm tra Status Code, JSON response và hiển thị kết quả tại tab **Test Results**.
4. **Chi tiết đầy đủ:** Xem tại tài liệu [`postman/HUONG_DAN_POSTMAN_VSCODE.md`](postman/HUONG_DAN_POSTMAN_VSCODE.md).

---

## 9. HƯỚNG DẪN CHẠY BÀI 2 VỀ NHÀ: FLASK LOCAL SERVER & CLIENT
Bài này cần mở **2 cửa sổ Terminal song song**:

### Bước 1: Mở Terminal 1 để chạy Flask Server
```powershell
# Di chuyển tới thư mục Lab04 và kích hoạt .venv (nếu có)
python bai_ve_nha/bai2_flask_server/server.py
```
Server sẽ lắng nghe tại: `http://127.0.0.1:5000/api/sensor`

### Bước 2: Mở Terminal 2 để chạy Client kiểm thử
```powershell
# Di chuyển tới thư mục Lab04 và kích hoạt .venv (nếu có)
python bai_ve_nha/bai2_flask_server/client.py
```
Client sẽ tự động gửi:
1. **Request 1**: Đầy đủ trường dữ liệu hợp lệ -> Server in dữ liệu và trả về `200 OK`.
2. **Request 2**: Cố tình thiếu trường `device_id` -> Server phát hiện và trả về lỗi `400 Bad Request`.

---

## 10. HƯỚNG DẪN CHẠY BÀI 3 VỀ NHÀ: ESP32 + DHT22 TRÊN WOKWI

1. Truy cập [https://wokwi.com](https://wokwi.com) -> Chọn **ESP32** -> **ESP32 (Arduino)**.
2. Dán nội dung các file từ thư mục `bai_ve_nha/bai3_esp32_wokwi/`:
   - Tab `sketch.ino`: Dán code từ file `sketch.ino` (Đã được cài sẵn URL Webhook cá nhân của bạn).
   - Tab `diagram.json`: Dán sơ đồ kết nối từ file `diagram.json`.
   - Tab `libraries.txt`: Nhập 3 thư viện:
     ```text
     DHT sensor library
     Adafruit Unified Sensor
     ArduinoJson
     ```
3. Bấm nút **Play (Start simulation)** màu xanh lá.
4. Mở **Serial Monitor** quan sát ESP32 kết nối Wi-Fi `Wokwi-GUEST` và gửi dữ liệu mỗi 10s.
5. Nhấp vào cảm biến DHT22 trên mạch để kéo thay đổi nhiệt độ / độ ẩm và xem dữ liệu cập nhật trên Webhook.site.

---

## 11. KẾT QUẢ MONG ĐỢI CỦA TỪNG BÀI

| Bài | Tên bài | Kết quả trên Console / Phần mềm | Kết quả trên Server (Webhook / Flask) |
| :--- | :--- | :--- | :--- |
| **Bài 1** | Postman POST | Postman trả về mã `200 OK`, thời gian phản hồi < 1s | Webhook.site ghi nhận gói POST với `{"device_id": "sensor_01", "temperature": 25.5}` |
| **Bài 2** | JSON Body | Postman gửi thành công không có lỗi cú pháp | Webhook.site nhận cấu trúc JSON đầy đủ 4 nhóm trường (`timestamp`, `node_id`, `sensors`, `battery_level`) |
| **Bài 3** | Python Requests | Terminal in URL, JSON payload gửi đi, mã `200 OK` | Webhook.site ghi nhận dữ liệu cảm biến mới nhất |
| **Bài 4** | Authentication | Terminal in danh sách các Header xác thực kèm Status `200` | Bảng Headers trên Webhook.site hiển thị `x-api-key` và `authorization: Bearer` |
| **Bài 5** | Timeout & Exception | Chương trình lần lượt kiểm thử 3 kịch bản: Hợp lệ (200), Timeout (sau 3s), Connection Error mà **không bị crash** | Console in thông báo tiếng Việt giải thích rõ từng mã lỗi kỹ thuật |
| **Về nhà 1** | Periodic Sender | Cứ 10 giây in một lần gửi (Temp, Humidity ngẫu nhiên); bấm `Ctrl + C` dừng an toàn | Webhook.site hiển thị các bản tin gửi nối tiếp nhau theo thời gian thực |
| **Về nhà 2** | Flask Server | Terminal Client in kết quả Request 1 (200 OK) và Request 2 (400 Bad Request) | Terminal Server in đầy đủ thông tin cảm biến nhận được từ Client |
| **Về nhà 3** | ESP32 Wokwi | Serial Monitor in `[Wi-Fi] Kết nối thành công!`, payload JSON và `HTTP Status Code: 200` | Webhook.site nhận dữ liệu thực tế từ thiết bị `esp32_dht22_wokwi` |

---

## 12. DANH SÁCH ẢNH CẦN CHỤP ĐỂ NỘP BÁO CÁO

*(Chi tiết vị trí và nội dung ảnh được mô tả tại file [`minh_chung/README.md`](minh_chung/README.md))*:
1. `bai1_postman_request.png`: Giao diện Postman gửi Bài 1 (Status 200 OK).
2. `bai1_webhook_result.png`: Webhook.site nhận dữ liệu Bài 1.
3. `bai2_postman_complex_json.png`: Postman gửi JSON đa cảm biến Bài 2.
4. `bai2_webhook_complex_json.png`: Webhook.site nhận JSON đa thông số Bài 2.
5. `bai3_python_run.png`: Terminal chạy Bài 3 thành công.
6. `bai3_webhook_python.png`: Webhook.site nhận dữ liệu từ Bài 3.
7. `bai4_python_run.png`: Terminal chạy Bài 4 hiển thị Headers.
8. `bai4_webhook_headers.png`: Webhook.site nhận đúng các Header xác thực (`X-API-Key`, `Authorization`).
9. `bai5_terminal_exceptions.png`: Terminal chạy Bài 5 bắt thành công 3 kịch bản lỗi.
10. `bai1_vn_periodic_running.png`: Terminal Bài 1 về nhà chạy liên tục nhiều chu kỳ 10 giây.
11. `bai1_vn_stop_ctrl_c.png`: Terminal Bài 1 về nhà khi nhấn `Ctrl + C` dừng an toàn.
12. `bai1_vn_webhook_stream.png`: Webhook.site nhận luồng dữ liệu định kỳ từ Bài 1 về nhà.
13. `bai2_vn_server_terminal.png`: Màn hình Terminal 1 chạy Flask Server Bài 2 về nhà.
14. `bai2_vn_client_terminal.png`: Màn hình Terminal 2 chạy Client gửi 200 OK và 400 Error.
15. `bai3_vn_wokwi_circuit.png`: Sơ đồ đấu nối mạch ESP32 và DHT22 trên Wokwi.
16. `bai3_vn_wokwi_serial.png`: Serial Monitor trên Wokwi in kết nối và gửi POST thành công.
17. `bai3_vn_webhook_esp32.png`: Webhook.site nhận dữ liệu gửi từ vi điều khiển ESP32.

---

## 13. CÁC LỖI THƯỜNG GẶP VÀ CÁCH KHẮC PHỤC

| Hiện tượng lỗi | Nguyên nhân | Cách khắc phục |
| :--- | :--- | :--- |
| `ModuleNotFoundError: No module named 'requests'` hoặc `'flask'` | Chưa kích hoạt môi trường ảo hoặc chưa cài đặt thư viện | Chạy `.venv\Scripts\activate` rồi chạy `pip install -r requirements.txt` |
| `requests.exceptions.ConnectionError: ... Failed to establish a new connection` | 1. Chưa khởi động Flask Server ở Bài 2 về nhà.<br>2. Sai cổng hoặc sai địa chỉ IP | Chạy file `server.py` ở một Terminal trước, sau đó mới chạy `client.py` |
| Status `404 Not Found` trên Webhook.site | URL Webhook bị sai hoặc phiên làm việc đã hết hạn | Truy cập lại [https://webhook.site](https://webhook.site) lấy URL mới và cập nhật vào `config.py` |
| JSON Syntax Error trong Postman | Thừa dấu phẩy ở cuối (`trailing comma`) hoặc dùng nháy đơn `'` | Đổi toàn bộ nháy đơn thành nháy kép `"`, xóa dấu phẩy ở thuộc tính cuối cùng |
| ESP32 trên Wokwi báo `[Wi-Fi] [CẢNH BÁO] Kết nối thất bại` | Nhầm SSID mạng thực tế của nhà thay vì mạng ảo của Wokwi | Giữ nguyên cấu hình mạng ảo của Wokwi: `ssid = "Wokwi-GUEST"` và `password = ""` |
| ESP32 trên Wokwi báo `Không thể đọc dữ liệu từ cảm biến DHT22` | Sai chân kết nối SDA của DHT22 | Kiểm tra `sketch.ino` đã khai báo `#define DHTPIN 15` và trong `diagram.json` chân `dht1:SDA` đã nối vào `esp:15` |

---

## 14. CÁCH DỪNG CHƯƠNG TRÌNH PERIODIC SENDER BẰNG CTRL + C
- Khi chương trình `periodic_sender.py` đang chạy vòng lặp gửi định kỳ, trỏ chuột vào màn hình Terminal.
- Nhấn tổ hợp phím **`Ctrl + C`** trên bàn phím.
- Chương trình được lập trình bọc trong khối `try ... except KeyboardInterrupt`, do đó sẽ ngắt một cách êm thuận, in lời chào tạm biệt và số lần đã gửi thành công trước khi thoát, hoàn toàn không gây treo máy hay lỗi tiến trình.


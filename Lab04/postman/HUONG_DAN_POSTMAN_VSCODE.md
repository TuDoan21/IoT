# HƯỚNG DẪN SỬ DỤNG POSTMAN EXTENSION TRÊN VISUAL STUDIO CODE (LAB 04)

Tài liệu này hướng dẫn chi tiết cách sử dụng **Postman Extension trực tiếp bên trong Visual Studio Code** để thực hành toàn bộ các bài tập gửi nhận HTTP POST Request trong **Lab 04 - Internet of Things (IoT)**.

---

## 1. TỔNG QUAN CÁC FILE ĐƯỢC CUNG CẤP

Trong thư mục `Lab04/postman/` đã chuẩn bị sẵn toàn bộ các tài nguyên cần thiết:

| Tên File | Mô tả chức năng |
| :--- | :--- |
| **`Lab04_HTTP_POST.postman_collection.json`** | Bộ sưu tập Postman Collection chứa 5 thư mục với đầy đủ các kịch bản request và Test Scripts tự động. |
| **`Lab04_Webhook.postman_environment.json`** | Môi trường Postman cho Webhook (đã cấu hình sẵn URL Webhook.site cá nhân của bạn). |
| **`Lab04_Local.postman_environment.json`** | Môi trường Postman kiểm thử máy chủ cục bộ Flask (`http://127.0.0.1:5000`). |
| **`lab04_requests.http`** | File kịch bản chạy trực tiếp bằng extension **REST Client** (tùy chọn nhanh không cần Postman). |
| **`HUONG_DAN_POSTMAN_VSCODE.md`** | Tài liệu hướng dẫn từng bước chi tiết (file này). |

---

## 2. CHUẨN BỊ MÔI TRƯỜNG TRÊN VS CODE

### Bước 1: Mở thư mục Lab04 bằng VS Code
1. Khởi động phần mềm **Visual Studio Code**.
2. Chọn menu **File** -> **Open Folder...** (hoặc phím tắt `Ctrl + K, Ctrl + O`).
3. Điều hướng và chọn thư mục `Lab04`.

### Bước 2: Mở giao diện Postman Extension
1. Nhìn vào thanh công cụ bên trái (Activity Bar) của VS Code.
2. Nhấn vào biểu tượng **Postman** (hình phi hành gia màu cam).
3. **Đăng nhập Postman:**
   - Nếu extension yêu cầu đăng nhập: Nhấn **Sign In**, trình duyệt sẽ mở trang đăng nhập Postman. Đăng nhập bằng tài khoản Google hoặc tài khoản Postman miễn phí.
   - Sau khi đăng nhập thành công, quay lại VS Code, extension sẽ hiển thị giao diện làm việc gồm: *Collections, Environments, History*.

---

## 3. IMPORT COLLECTION VÀ ENVIRONMENT VÀO POSTMAN VS CODE

### Bước 1: Import Postman Collection
1. Tại khung điều hướng Postman bên trái VS Code, di chuột vào mục **Collections** và nhấn biểu tượng **Import** (hoặc dấu `+` / menu ba chấm `...` -> **Import**).
2. Chọn **File** -> Tìm tới đường dẫn:
   ```text
   Lab04/postman/Lab04_HTTP_POST.postman_collection.json
   ```
3. Nhấn **Import**. Bạn sẽ thấy Collection **`Lab04 HTTP POST và IoT Sensor`** xuất hiện với 5 thư mục con:
   - `01 HTTP POST cơ bản`
   - `02 JSON cảm biến nâng cao`
   - `03 Authentication`
   - `04 Timeout và kiểm tra lỗi`
   - `05 Flask Local Server`

### Bước 2: Import Postman Environments
1. Tại khung điều hướng Postman, chọn mục **Environments**.
2. Nhấn nút **Import** (hoặc biểu tượng dấu `+`).
3. Lần lượt chọn và import 2 file môi trường:
   - `Lab04/postman/Lab04_Webhook.postman_environment.json` (Môi trường Webhook)
   - `Lab04/postman/Lab04_Local.postman_environment.json` (Môi trường Local Server)
4. Sau khi import, bạn sẽ có 2 môi trường là **Lab04 Webhook** và **Lab04 Local**.

---

## 4. CẤU HÌNH VÀ CHỌN ENVIRONMENT PHÙ HỢP

### Cách chọn Environment khi gửi Request
- Ở góc trên bên phải của tab request trong Postman VS Code có menu xổ xuống chọn Environment (mặc định hiển thị *No Environment*).
- **Khi gửi request tới Webhook.site (Thư mục 01, 02, 03, 04):**
  -> Chọn environment: **`Lab04 Webhook`**.
- **Khi gửi request tới Flask Server nội bộ (Thư mục 05):**
  -> Chọn environment: **`Lab04 Local`**.

### Cách xem và cập nhật URL Webhook.site
File `Lab04_Webhook.postman_environment.json` đã được cài đặt sẵn URL Webhook.site cá nhân của bạn:
```text
https://webhook.site/7e245820-4dfb-425a-a195-59e69a15b129
```
Nếu sau này bạn muốn đổi sang một Webhook URL mới:
1. Mở trình duyệt truy cập: [https://webhook.site](https://webhook.site).
2. Bấm nút **Copy to clipboard** tại ô **Your unique URL**.
3. Trong Postman VS Code, bấm vào **Environments** -> chọn **Lab04 Webhook**.
4. Sửa giá trị của biến `webhook_url` thành URL mới -> Nhấn **Save** (`Ctrl + S`).

---

## 5. THỰC HIỆN TỪNG REQUEST VÀ QUAN SÁT KẾT QUẢ

### 01. HTTP POST cơ bản
1. Mở thư mục `01 HTTP POST cơ bản` -> Chọn request **POST dữ liệu cảm biến đơn giản**.
2. Đảm bảo Environment đang là **Lab04 Webhook**.
3. Bấm nút **Send** (màu xanh dương).
4. **Quan sát kết quả:**
   - **Status Code**: Hiển thị `200 OK`.
   - **Tab Test Results**: Hiển thị `PASS (3/3)` gồm:
     - `Status code is 2xx success`
     - `Response time is under 5000ms`
     - `Response body is present`
5. Mở trình duyệt Webhook.site, bạn sẽ thấy request xuất hiện ngay lập tức với body:
   ```json
   {
     "device_id": "sensor_01",
     "temperature": 25.5
   }
   ```

### 02. JSON cảm biến nâng cao
1. Mở thư mục `02 JSON cảm biến nâng cao` -> Chọn **POST dữ liệu cảm biến đầy đủ**.
2. Kiểm tra tab **Body**: Sử dụng biến động `{{$isoTimestamp}}` và `{{$randomInt}}`.
3. Bấm nút **Send**.
4. Kiểm tra trên Webhook.site: Các trường `timestamp`, `node_id`, `light_intensity` được sinh tự động ngẫu nhiên và đúng thời gian thực.

### 03. Authentication
1. Mở thư mục `03 Authentication`:
   - **POST với API Key**: Gửi kèm Header `X-API-Key: my_secret_key_123`.
   - **POST với Bearer Token**: Gửi kèm Header `Authorization: Bearer token_xyz`.
   - **POST kết hợp API Key và Bearer Token**: Gửi đồng thời cả 2 header.
2. Chọn từng request và bấm **Send**.
3. Trên Webhook.site, nhấn vào request vừa nhận và xem mục **Headers**:
   - Xác nhận có `x-api-key: my_secret_key_123`.
   - Xác nhận có `authorization: Bearer token_xyz`.

### 04. Timeout và kiểm tra lỗi
1. **POST hợp lệ kiểm tra Webhook**: Bấm **Send** -> Nhận kết quả thành công (200 OK).
2. **POST URL lỗi mô phỏng mất kết nối (Chỉ kiểm thử ngoại lệ)**:
   - Bấm **Send**.
   - Postman sẽ báo lỗi kết nối mạng (ví dụ: *Could not get response*, *ETIMEDOUT* hoặc *ECONNREFUSED*).
   - **Giải thích:** Request này cố tình trỏ tới địa chỉ IP không tồn tại để sinh viên quan sát phản ứng của client khi mạng gặp sự cố.

### 05. Flask Local Server
Trước khi gửi các request trong thư mục này, bạn **BẮT BUỘC** phải bật server:

1. **Khởi động Flask Server:**
   Mở một Terminal trong VS Code (`Ctrl + ` `~`) và chạy:
   ```powershell
   python bai_ve_nha/bai2_flask_server/server.py
   ```
   Console hiển thị: `* Running on http://127.0.0.1:5000`.

2. **Chuyển Environment sang:** **`Lab04 Local`**.

3. **Thực hiện 3 request:**
   - **GET kiểm tra trạng thái Server (Health Check)**:
     - Bấm **Send** -> Nhận `200 OK`, body trả về:
       ```json
       {
         "message": "Flask sensor server is running",
         "status": "healthy"
       }
       ```
     - Test Results: `PASS (4/4)`.
   - **POST dữ liệu cảm biến hợp lệ (HTTP 200)**:
     - Bấm **Send** -> Nhận `200 OK`, body trả về:
       ```json
       {
         "message": "Data received successfully",
         "status": "success"
       }
       ```
     - Terminal chạy `server.py` in ra dữ liệu cảm biến nhận được.
   - **POST dữ liệu thiếu trường (Kiểm tra HTTP 400)**:
     - Bấm **Send** -> Nhận `400 BAD REQUEST`, body trả về:
       ```json
       {
         "message": "Thiếu trường bắt buộc 'device_id' trong dữ liệu cảm biến!",
         "status": "error"
       }
       ```
     - Test Results: `PASS (4/4)` (Kiểm tra `pm.response.to.have.status(400)` đạt yêu cầu).

---

## 6. DANH SÁCH ẢNH CHỤP MÀN HÌNH CẦN LÀM MINH CHỨNG (BÁO CÁO)

Khi làm báo cáo bài tập, bạn hãy chụp các ảnh màn hình sau và lưu vào thư mục `Lab04/minh_chung/`:

1. **Minh chứng 1:** Giao diện Postman trên VS Code sau khi import Collection `Lab04 HTTP POST và IoT Sensor` và chọn Environment `Lab04 Webhook`.
2. **Minh chứng 2:** Tab Response và Test Results của request **POST dữ liệu cảm biến đơn giản** (hiển thị Status 200 OK và PASS 3/3).
3. **Minh chứng 3:** Tab Response và Test Results của request **POST dữ liệu cảm biến đầy đủ** (JSON nâng cao).
4. **Minh chứng 4:** Màn hình Webhook.site mục **Headers** chứng minh nhận thành công `X-API-Key` và `Authorization: Bearer`.
5. **Minh chứng 5:** Tab Response của request **POST dữ liệu thiếu trường** tới Flask Local Server (hiển thị Status 400 Bad Request và PASS của Test Script).
6. **Minh chứng 6:** Terminal VS Code chạy Flask `server.py` ghi nhận các gói tin POST gửi từ Postman.

---

## 7. CÁCH DÙNG FILE `lab04_requests.http` VỚI REST CLIENT (TÙY CHỌN)

Nếu bạn muốn gửi request siêu nhanh mà không cần mở giao diện Postman:
1. Cài đặt extension **REST Client** (tác giả Huachao Mao) từ VS Code Extensions Marketplace (`Ctrl + Shift + X` -> tìm `REST Client`).
2. Mở file `Lab04/postman/lab04_requests.http`.
3. Phía trên mỗi request sẽ xuất hiện dòng chữ nhỏ **`Send Request`**.
4. Nhấp chuột vào **`Send Request`**, kết quả phản hồi sẽ hiển thị ngay ở tab bên phải.

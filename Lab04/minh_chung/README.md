# TỔNG HỢP DANH SÁCH ẢNH MINH CHỨNG THỰC HÀNH LAB 04

Thư mục này dùng để lưu trữ toàn bộ ảnh chụp màn hình làm minh chứng cho báo cáo kết quả thực hành môn Internet of Things (IoT) - Lab 04.

---

## BẢNG DANH MỤC ẢNH MINH CHỨNG CẦN CHỤP

| STT | Bài thực hành | Tên file ảnh gợi ý | Nội dung cần thể hiện trong ảnh |
| :---: | :--- | :--- | :--- |
| **1** | **Bài 1 (Trên lớp)**<br>Postman & Webhook | `bai1_postman_request.png` | Giao diện Postman gửi POST thành công, hiển thị URL Webhook, Header `application/json`, Body JSON đơn giản, và Status `200 OK`. |
| **2** | **Bài 1 (Trên lớp)**<br>Postman & Webhook | `bai1_webhook_result.png` | Giao diện Webhook.site hiển thị request POST nhận được với đúng nội dung Body JSON mẫu. |
| **3** | **Bài 2 (Trên lớp)**<br>JSON Đa Cảm Biến | `bai2_postman_complex_json.png` | Postman gửi thành công cấu trúc JSON đa cảm biến (`timestamp`, `node_id`, `sensors`, `battery_level`). |
| **4** | **Bài 2 (Trên lớp)**<br>JSON Đa Cảm Biến | `bai2_webhook_complex_json.png` | Webhook.site hiển thị dữ liệu JSON đa thông số được phân tích cú pháp chuẩn xác. |
| **5** | **Bài 3 (Trên lớp)**<br>Python Requests | `bai3_python_run.png` | Màn hình Terminal chạy file `send_sensor.py` thành công, in ra URL, JSON gửi và Status `200 OK`. |
| **6** | **Bài 3 (Trên lớp)**<br>Python Requests | `bai3_webhook_python.png` | Giao diện Webhook.site ghi nhận request gửi lên từ chương trình Python. |
| **7** | **Bài 4 (Trên lớp)**<br>Authentication | `bai4_python_run.png` | Màn hình Terminal chạy file `send_with_auth.py` in chi tiết các Headers xác thực đã gửi. |
| **8** | **Bài 4 (Trên lớp)**<br>Authentication | `bai4_webhook_headers.png` | Bảng Headers trên Webhook.site thể hiện rõ `x-api-key` và `authorization: Bearer ...`. |
| **9** | **Bài 5 (Trên lớp)**<br>Timeout & Exception | `bai5_terminal_exceptions.png` | Terminal chạy file `send_with_error_handling.py` hiển thị bắt thành công các kịch bản (Thành công, Timeout sau 3s, Connection Error) mà không bị crash. |
| **10** | **Bài 1 (Về nhà)**<br>Periodic Sender | `bai1_vn_periodic_running.png` | Terminal chạy `periodic_sender.py` gửi dữ liệu định kỳ mỗi 10 giây (hiển thị ít nhất 2 - 3 chu kỳ gửi liên tiếp). |
| **11** | **Bài 1 (Về nhà)**<br>Periodic Sender | `bai1_vn_stop_ctrl_c.png` | Màn hình Terminal khi nhấn `Ctrl + C`, hiển thị thông báo dừng an toàn chương trình. |
| **12** | **Bài 1 (Về nhà)**<br>Periodic Sender | `bai1_vn_webhook_stream.png` | Webhook.site hiển thị luồng dữ liệu nhận được liên tục từ các chu kỳ khác nhau. |
| **13** | **Bài 2 (Về nhà)**<br>Flask Local Server | `bai2_vn_server_terminal.png` | Terminal 1 chạy `server.py` hiển thị log tiếp nhận request và in dữ liệu cảm biến ra màn hình. |
| **14** | **Bài 2 (Về nhà)**<br>Flask Local Server | `bai2_vn_client_terminal.png` | Terminal 2 chạy `client.py` hiển thị gửi dữ liệu thành công (200 OK) và kịch bản phát hiện dữ liệu thiếu (400 Bad Request). |
| **15** | **Bài 3 (Về nhà)**<br>ESP32 Wokwi | `bai3_vn_wokwi_circuit.png` | Sơ đồ đấu nối giữa ESP32 và cảm biến DHT22 trên trình mô phỏng Wokwi. |
| **16** | **Bài 3 (Về nhà)**<br>ESP32 Wokwi | `bai3_vn_wokwi_serial.png` | Màn hình Serial Monitor trên Wokwi hiển thị quá trình kết nối Wi-Fi, đọc DHT22 và gửi POST thành công (200 OK). |
| **17** | **Bài 3 (Về nhà)**<br>ESP32 Wokwi | `bai3_vn_webhook_esp32.png` | Webhook.site ghi nhận dữ liệu nhiệt độ, độ ẩm gửi lên từ thiết bị mô phỏng ESP32. |

---

## HƯỚNG DẪN LƯU VÀ CHÈN ẢNH VÀO BÁO CÁO WORD
1. Sử dụng công cụ chụp ảnh màn hình của Windows: tổ hợp phím **`Windows + Shift + S`** hoặc phần mềm **Snipping Tool**.
2. Lưu ảnh trực tiếp vào thư mục `Lab04/minh_chung/` theo đúng tên gợi ý ở bảng trên.
3. Khi làm báo cáo bằng Word, chèn các ảnh này vào từng mục bài tương ứng kèm chú thích rõ ràng.

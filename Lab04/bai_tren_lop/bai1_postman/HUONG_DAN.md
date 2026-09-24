# HƯỚNG DẪN THỰC HÀNH BÀI 1: KHẢO SÁT HTTP POST VÀ POSTMAN CƠ BẢN

## 1. Mục tiêu bài học
- Hiểu rõ cấu trúc cơ bản của một gói tin **HTTP POST Request** (Method, Headers, Body).
- Thành thạo việc sử dụng công cụ **Postman** để gửi request thủ công tới một dịch vụ nhận dữ liệu trên Internet.
- Biết cách sử dụng **Webhook.site** để bắt và kiểm tra trực tiếp các gói tin truyền từ thiết bị IoT hoặc ứng dụng.

---

## 2. Các bước thực hiện chi tiết

### Bước 1: Lấy URL kiểm thử miễn phí từ Webhook.site
1. Mở trình duyệt web và truy cập vào trang: [https://webhook.site](https://webhook.site)
2. Trang web sẽ tự động sinh ra cho bạn một URL duy nhất.
   - Nhìn vào mục **"Your unique URL"** (ví dụ: `https://webhook.site/3f68a2d1-9b1e-450b-9dfc-112233445566`).
3. Bấm vào nút **Copy to clipboard** bên cạnh URL này để lưu lại sử dụng.
4. Giữ nguyên tab trình duyệt này để quan sát dữ liệu gửi đến ở các bước tiếp theo.

---

### Bước 2: Tạo HTTP POST Request trong Postman
1. Mở ứng dụng **Postman** trên máy tính của bạn.
2. Nhấn vào nút **+** (New Tab) để tạo một Request mới.
3. Tại ô chọn phương thức (Method - mặc định là `GET`), nhấp chuột và đổi thành **`POST`**.
4. Dán URL đã sao chép từ Webhook.site ở Bước 1 vào ô nhập địa chỉ:
   ```text
   https://webhook.site/7e245820-4dfb-425a-a195-59e69a15b129
   ```

---

### Bước 3: Thiết lập Headers cho Request
1. Trong Postman, chọn tab **Headers** (nằm ngay phía dưới thanh nhập URL).
2. Thêm một cặp Key - Value mới:
   - **Key**: `Content-Type`
   - **Value**: `application/json`
3. *(Lưu ý: Header này báo cho server biết rằng phần thân dữ liệu gửi đi được định dạng theo chuẩn JSON).*

---

### Bước 4: Soạn thảo JSON Body giả lập dữ liệu cảm biến
1. Trong Postman, chuyển sang tab **Body**.
2. Chọn tùy chọn **raw**.
3. Tại menu chọn định dạng ở góc phải của dòng tùy chọn, chọn **JSON**.
4. Nhập nội dung JSON mẫu sau vào vùng soạn thảo:
   ```json
   {
     "device_id": "sensor_01",
     "temperature": 25.5
   }
   ```

---

### Bước 5: Gửi Request và Kiểm tra kết quả
1. Nhấn nút **Send** màu xanh ở góc trên bên phải của Postman.
2. **Quan sát phản hồi (Response) trên Postman:**
   - **Status Code**: Hiển thị `200 OK` (hoặc mã thành công tương đương).
   - **Time**: Thời gian gửi và nhận phản hồi (ví dụ: ~200 - 500 ms).
   - **Response Body**: Hiển thị nội dung phản hồi từ server Webhook.site.
3. **Kiểm tra trên giao diện Webhook.site:**
   - Quay lại tab trình duyệt Webhook.site.
   - Nhìn sang cột bên trái (Requests), bạn sẽ thấy một request POST mới xuất hiện.
   - Bấm vào request đó và kiểm tra:
     - **Method**: `POST`
     - **Headers**: Kiểm tra có header `content-type: application/json`.
     - **Raw Content** / **JSON Formatted**: Hiển thị đúng chuỗi JSON bạn vừa gửi:
       ```json
       {
         "device_id": "sensor_01",
         "temperature": 25.5
       }
       ```

---

## 3. Danh sách ảnh chụp màn hình cần làm minh chứng (Minh Chứng)
Để nộp báo cáo thực hành bài 1, bạn cần chụp **02 ảnh**:

1. **Ảnh 1 - Giao diện Postman gửi thành công:**
   - Thể hiện rõ: Method `POST`, URL Webhook của bạn, Header `Content-Type: application/json`, Body JSON mẫu, và phần Response bên dưới hiển thị `Status: 200 OK`.
   - Lưu tên file gợi ý: `minh_chung/bai1_postman_request.png`

2. **Ảnh 2 - Giao diện Webhook.site nhận dữ liệu:**
   - Thể hiện rõ request POST bên cột trái, phần Headers và nội dung JSON nhận được đúng với dữ liệu bạn đã gửi từ Postman.
   - Lưu tên file gợi ý: `minh_chung/bai1_webhook_result.png`

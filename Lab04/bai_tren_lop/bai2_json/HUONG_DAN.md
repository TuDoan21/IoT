# HƯỚNG DẪN THỰC HÀNH BÀI 2: MÔ PHỎNG DỮ LIỆU CẢM BIẾN ĐA DẠNG VỚI JSON BODY

## 1. Mục tiêu bài học
- Nắm vững cấu trúc cú pháp của chuẩn định dạng dữ liệu **JSON** (JavaScript Object Notation).
- Xây dựng cấu trúc dữ liệu JSON thực tế cho nút cảm biến IoT đa năng (nhiệt độ, độ ẩm, cường độ sáng, pin, thời gian).
- Biết cách phát hiện, giải thích và sửa các lỗi cú pháp JSON thường gặp khi làm việc với API.

---

## 2. Cấu trúc JSON cảm biến mẫu (`sensor_data.json`)
Cấu trúc chuẩn đã được tạo trong file `sensor_data.json`:
```json
{
  "timestamp": "2026-09-17T14:30:00Z",
  "node_id": "esp32_sensor_node_01",
  "sensors": {
    "temperature": 28.75,
    "humidity": 68.2,
    "light_intensity": 520
  },
  "battery_level": 94.5
}
```

### Ý nghĩa từng trường dữ liệu:
| Tên trường | Kiểu dữ liệu | Ý nghĩa | Ví dụ giá trị |
| :--- | :--- | :--- | :--- |
| `timestamp` | String (ISO 8601) | Thời điểm cảm biến ghi nhận dữ liệu | `"2026-09-17T14:30:00Z"` |
| `node_id` | String | Mã định danh duy nhất của nút cảm biến | `"esp32_sensor_node_01"` |
| `sensors` | Object (Dictionary) | Đối tượng lồng nhau chứa thông số các cảm biến | `{ ... }` |
| `sensors.temperature` | Number (Float) | Nhiệt độ môi trường (°C) | `28.75` |
| `sensors.humidity` | Number (Float) | Độ ẩm không khí (%) | `68.2` |
| `sensors.light_intensity`| Number (Integer) | Cường độ ánh sáng (Lux) | `520` |
| `battery_level` | Number (Float) | Mức dung lượng pin còn lại (%) | `94.5` |

---

## 3. Các bước thực hành gửi dữ liệu trên Postman

1. Mở Postman, tạo một Request mới hoặc dùng lại tab từ Bài 1.
2. Thiết lập:
   - **Method**: `POST`
   - **URL**: Dán URL Webhook.site của bạn (`https://webhook.site/7e245820-4dfb-425a-a195-59e69a15b129`).
   - **Headers**: `Content-Type: application/json`
3. Chuyển sang tab **Body** -> Chọn **raw** -> Chọn định dạng **JSON**.
4. Sao chép toàn bộ nội dung trong file `sensor_data.json` và dán vào khung soạn thảo Body.
5. Nhấn nút **Send**.
6. **Kiểm tra kết quả:**
   - Postman hiển thị: `Status: 200 OK`.
   - Webhook.site hiển thị request mới với đúng cấu trúc JSON gồm object lồng nhau `sensors`.

---

## 4. Cách phát hiện và khắc phục các lỗi cú pháp JSON thường gặp

Khi làm việc với JSON trong IoT và Web API, sinh viên rất thường gặp các lỗi sau:

### Lỗi 1: Dấu phẩy thừa ở phần tử cuối cùng (Trailing Comma)
- **Sai:**
  ```json
  {
    "temperature": 28.5,
    "humidity": 68.0,   <--- Dấu phẩy thừa ở cuối
  }
  ```
- **Hiện tượng:** Postman hoặc trình biên dịch báo lỗi đỏ `Unexpected token }`.
- **Cách khắc phục:** Xóa dấu phẩy sau trường dữ liệu cuối cùng trong một Object hoặc Array.

### Lỗi 2: Sử dụng nháy đơn `'` thay vì nháy kép `"`
- **Sai:**
  ```json
  {
    'node_id': 'esp32_01'
  }
  ```
- **Quy chuẩn JSON:** JSON chỉ chấp nhận dấu nháy kép `"`. Dấu nháy đơn `'` là sai chuẩn cú pháp.
- **Cách khắc phục:** Đổi toàn bộ nháy đơn thành nháy kép: `{"node_id": "esp32_01"}`.

### Lỗi 3: Thiếu dấu ngoặc kép quanh Key (Tên trường)
- **Sai:**
  ```json
  {
    temperature: 28.5
  }
  ```
- **Hiện tượng:** Trong Javascript thì hợp lệ, nhưng trong chuẩn JSON bắt buộc mọi key phải bọc trong `""`.
- **Cách khắc phục:** Thêm ngoặc kép quanh key: `"temperature": 28.5`.

### Lỗi 4: Không cân bằng dấu ngoặc `{}` hoặc `[]`
- **Hiện tượng:** Thiếu dấu đóng `}` của object con `sensors` hoặc của toàn bộ JSON body.
- **Cách phát hiện:**
  - Nhìn vào cột số dòng trong Postman / VSCode, nếu xuất hiện biểu tượng dấu chéo đỏ ❌ hoặc gạch chân đỏ ziczac tức là cú pháp đang bị lỗi.
  - Rê chuột vào vị trí báo lỗi để đọc gợi ý sửa chữa.

---

## 5. Danh sách ảnh chụp màn hình cần làm minh chứng (Minh Chứng)
Bạn cần chụp **02 ảnh**:

1. **Ảnh 1 - Postman gửi JSON đa thông số thành công:**
   - Thể hiện rõ: Body JSON đầy đủ 4 nhóm trường (`timestamp`, `node_id`, `sensors`, `battery_level`) và status code `200 OK`.
   - Lưu file: `minh_chung/bai2_postman_complex_json.png`

2. **Ảnh 2 - Webhook.site nhận cấu trúc JSON đầy đủ:**
   - Thể hiện rõ giao diện Webhook.site hiển thị cấu trúc JSON đã được format đẹp mắt với các thông số nhiệt độ, độ ẩm, ánh sáng, pin.
   - Lưu file: `minh_chung/bai2_webhook_complex_json.png`

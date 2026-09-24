# Bài Về Nhà 02: Last Will and Testament (LWT)

## 1. Mục tiêu
- Hiểu và ứng dụng cơ chế **Last Will and Testament (LWT - Di chúc)** trong giao thức MQTT nhằm giám sát trạng thái sống/chết (liveness) của thiết bị IoT.
- Cấu hình thông điệp LWT trước khi kết nối vào broker với payload `"Offline"`, cờ `retained = true` và `QoS = 1`.
- Sau khi kết nối thành công, lập trình ESP32 tự động publish trạng thái `"Online"` (kèm `retained = true`) lên cùng topic `lab05/doantuantu/status`.
- Thực nghiệm kịch bản ngắt kết nối đột ngột (rút nguồn / ngắt cáp USB) và quan sát broker tự động phát thông điệp "Offline" thay mặt thiết bị.

## 2. Phần cứng yêu cầu
- 01 board ESP32 DevKit V1.
- Cáp kết nối máy tính.

## 3. Cấu hình phần mềm
Tạo file `include/config.h` từ `include/config.example.h`:
```cpp
#define WIFI_SSID "Wokwi-GUEST"          // hoặc Wi-Fi thật của bạn
#define WIFI_PASSWORD ""
#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "esp32-doantuantu-lwt"
#define TOPIC_PREFIX "lab05/doantuantu"
#define TOPIC_STATUS "lab05/doantuantu/status"
#define LWT_PAYLOAD_OFFLINE "Offline"
#define LWT_PAYLOAD_ONLINE  "Online"
#define LWT_QOS 1
#define LWT_RETAIN true
```

## 4. Cách chạy và nạp chương trình
### Trên VS Code:
```bash
pio run -d VeNha/Bai02_MQTT_LWT
pio run -d VeNha/Bai02_MQTT_LWT -t upload
pio run -d VeNha/Bai02_MQTT_LWT -t monitor
```

## 5. Nguyên lý LWT & Hướng dẫn kiểm nghiệm thực tế
### Phân biệt ngắt kết nối đột ngột và ngắt thông thường:
- **Ngắt thông thường (Graceful Disconnect)**: Khi client chủ động gửi gói tin `DISCONNECT` tới broker, broker hiểu đây là hành vi có chủ đích và sẽ **hủy bỏ** thông điệp LWT đã đăng ký (không phát ra).
- **Ngắt đột ngột (Abrupt / Ungraceful Disconnect)**: Khi ESP32 bị sập nguồn đột ngột, đứt cáp hoặc mất sóng Wi-Fi, kết nối TCP bị đứt mà không có gói tin `DISCONNECT`. Broker phát hiện mất kết nối sẽ lập tức thực thi "di chúc", tự động gửi bản tin `Offline` tới topic `status`.

> [!NOTE]
> **Thời gian chờ Keepalive / Timeout**:
> - Bản tin `Offline` **KHÔNG** xuất hiện ngay tức thì (0 giây) khi vừa rút nguồn.
> - Broker cần đợi hết chu kỳ `Keep Alive` (mặc định của PubSubClient thường là 15 giây) nhân với hệ số an toàn (thường là 1.5 lần, khoảng 20 – 30 giây) để xác nhận kết nối TCP đã thực sự bị treo. Sau khoảng thời gian này, broker mới công bố trạng thái `Offline`.

### Các bước kiểm nghiệm trên MQTT Explorer:
1. Mở MQTT Explorer và kết nối đến `broker.hivemq.com:1883`.
2. Subscribe topic: `lab05/doantuantu/status` (hoặc `lab05/doantuantu/#`).
3. Cấp nguồn cho ESP32 chạy chương trình.
4. **Quan sát bước 1**: Topic `lab05/doantuantu/status` nhận được giá trị `"Online"`. Nhờ cờ `retained = true`, bất kỳ máy khách nào mở sau cũng sẽ đọc được ngay trạng thái "Online" này.
5. **Thực hiện ngắt kết nối đột ngột**: Rút hẳn cáp USB của ESP32 khỏi máy tính.
6. **Quan sát bước 2**: Nhìn vào MQTT Explorer và đếm khoảng 15 – 30 giây. Giá trị topic `lab05/doantuantu/status` sẽ tự động chuyển sang màu đỏ và đổi thành `"Offline"`.

## 6. Kết quả mong đợi trên Serial Monitor
```text
==================================================
 Lab 05 - Về Nhà 02: Last Will and Testament (LWT)
==================================================
[WiFi] Đang kết nối tới: Wokwi-GUEST
...
[WiFi] Đã kết nối! IP: 10.0.1.15
--------------------------------------------------
[MQTT] Đang kết nối tới broker: broker.hivemq.com
[MQTT] Cấu hình LWT (Di chúc) : Topic="lab05/doantuantu/status", Payload="Offline", QoS=1, Retained=true
[MQTT] KẾT NỐI BROKER THÀNH CÔNG!
[MQTT] Đang publish trạng thái trực tuyến: "Online" (retained = true)...
[MQTT] -> Publish "Online" THÀNH CÔNG!
*(Khi ESP32 mất nguồn hoặc mất mạng đột ngột, broker sẽ tự động phát "Offline")*
--------------------------------------------------
[Heartbeat] ESP32 đang hoạt động trực tuyến (Online) | Uptime: 10s
[Heartbeat] ESP32 đang hoạt động trực tuyến (Online) | Uptime: 20s
```

## 7. Minh chứng cần chụp ảnh báo cáo
1. **Ảnh chụp màn hình Terminal**: Biên dịch thành công với PlatformIO.
2. **Ảnh chụp Serial Monitor**: Hiển thị rõ cấu hình LWT và lệnh publish `Online` thành công.
3. **Ảnh chụp MQTT Explorer khi ESP32 đang Online**: Topic `lab05/doantuantu/status` hiển thị giá trị `Online`.
4. **Ảnh chụp MQTT Explorer sau khi rút nguồn ESP32 (30 giây)**: Topic `lab05/doantuantu/status` tự động chuyển thành `Offline`.

## 8. Các lỗi thường gặp và cách xử lý
- **Rút nguồn mà mãi không thấy `Offline` xuất hiện**:
  - Hãy kiên nhẫn chờ từ 30 đến 45 giây do cơ chế TCP timeout của broker công cộng.
  - Kiểm tra xem lúc gọi hàm `connect()`, bạn đã truyền đúng 5 đối số: `connect(id, willTopic, willQos, willRetain, willMessage)` hay chưa.
- **Client khác mới vào không thấy trạng thái Online**: Do quên đặt cờ `retained = true` khi publish `"Online"`.

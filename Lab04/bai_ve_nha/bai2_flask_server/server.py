# -*- coding: utf-8 -*-
"""
BÀI VỀ NHÀ 2: LOCAL FLASK TEST SERVER
-------------------------------------------------------------------------
Chương trình tạo một REST API server cục bộ bằng Flask:
- Endpoint: POST http://127.0.0.1:5000/api/sensor
- Kiểm tra tính hợp lệ của Header Content-Type và định dạng JSON
- Kiểm tra các trường dữ liệu cảm biến cần thiết (device_id, temperature, ...)
- In thông tin nhận được ra console
- Trả về JSON 200 nếu hợp lệ hoặc 400 nếu sai định dạng / thiếu dữ liệu.
-------------------------------------------------------------------------
"""

import sys
import logging
from flask import Flask, request, jsonify

# Tự động cấu hình chuẩn UTF-8 cho console Windows tránh lỗi Unicode
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8")
if hasattr(sys.stderr, "reconfigure"):
    sys.stderr.reconfigure(encoding="utf-8")

# Tắt bớt log không cần thiết của werkzeug để console gọn gàng hơn
log = logging.getLogger('werkzeug')
log.setLevel(logging.INFO)

app = Flask(__name__)

# Thiết lập UTF-8 cho response JSON của Flask
app.config['JSON_AS_ASCII'] = False


@app.route('/', methods=['GET'])
def index():
    """
    Trang chủ hiển thị thông tin máy chủ khi mở trên trình duyệt.
    """
    return jsonify({
        "server": "Flask Local Sensor Server (Lab 04)",
        "status": "online",
        "endpoints": {
            "health_check": "GET /health",
            "receive_sensor": "POST /api/sensor (Gửi dữ liệu JSON qua Postman hoặc REST Client)"
        }
    }), 200


@app.route('/api/sensor', methods=['POST'])
def receive_sensor_data():
    """
    Route tiếp nhận dữ liệu cảm biến gửi lên từ Client.
    Chỉ chấp nhận phương thức POST và Content-Type: application/json.
    """
    print("\n" + "=" * 60)
    print("[*] SERVER NHẬN ĐƯỢC MỘT REQUEST MỚI")
    print(f"[*] Client IP: {request.remote_addr}")
    print(f"[*] Content-Type: {request.headers.get('Content-Type')}")

    # 1. Kiểm tra xem request có chứa payload JSON hợp lệ không
    if not request.is_json:
        error_msg = "Request không hợp lệ. Content-Type phải là 'application/json'!"
        print(f"[!] LỖI 400: {error_msg}")
        return jsonify({
            "status": "error",
            "message": error_msg
        }), 400

    data = request.get_json(silent=True)
    if data is None:
        error_msg = "Dữ liệu JSON bị lỗi cú pháp hoặc rỗng!"
        print(f"[!] LỖI 400: {error_msg}")
        return jsonify({
            "status": "error",
            "message": error_msg
        }), 400

    # 2. Kiểm tra các trường dữ liệu tối thiểu
    if "device_id" not in data:
        error_msg = "Thiếu trường bắt buộc 'device_id' trong dữ liệu cảm biến!"
        print(f"[!] LỖI 400: {error_msg}")
        return jsonify({
            "status": "error",
            "message": error_msg
        }), 400

    # 3. In dữ liệu cảm biến nhận được ra màn hình console
    print("[+] DỮ LIỆU CẢM BIẾN NHẬN THÀNH CÔNG:")
    for key, value in data.items():
        print(f"    - {key}: {value}")

    # 4. Trả về phản hồi thành công theo đúng yêu cầu đề bài (HTTP 200)
    response_payload = {
        "status": "success",
        "message": "Data received successfully"
    }
    print("[+] Phản hồi 200 OK gửi lại cho Client.")
    print("=" * 60 + "\n")

    return jsonify(response_payload), 200


@app.route('/health', methods=['GET'])
def health_check():
    """
    Route kiểm tra trạng thái hoạt động của Server (Health Check).
    """
    return jsonify({
        "status": "healthy",
        "message": "Flask sensor server is running"
    }), 200



if __name__ == "__main__":
    HOST = "127.0.0.1"
    PORT = 5000

    print("=" * 65)
    print("          FLASK LOCAL SENSOR SERVER ĐANG KHỞI CHẠY              ")
    print("=" * 65)
    print(f"[*] Địa chỉ Server: http://{HOST}:{PORT}")
    print(f"[*] Endpoint nhận dữ liệu: http://{HOST}:{PORT}/api/sensor")
    print("[*] Phương thức hỗ trợ: POST")
    print("[*] Nhấn [Ctrl + C] để dừng Server bất cứ lúc nào.")
    print("=" * 65)

    # Chạy server với debug=False theo yêu cầu
    app.run(host=HOST, port=PORT, debug=False)

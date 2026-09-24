# -*- coding: utf-8 -*-
"""
BÀI VỀ NHÀ 2: CLIENT GỬI DỮ LIỆU TỚI LOCAL FLASK SERVER
-------------------------------------------------------------------------
Chương trình client gửi dữ liệu cảm biến tới Flask Server cục bộ:
- Đích: http://127.0.0.1:5000/api/sensor
- In chi tiết JSON gửi, mã HTTP status code và JSON phản hồi từ Server.
- Bao gồm cả kịch bản gửi hợp lệ (200 OK) và thử nghiệm gửi lỗi (400 Bad Request).
-------------------------------------------------------------------------
"""

import os
import sys
import json
import datetime
import requests

# Tự động cấu hình chuẩn UTF-8 cho console Windows tránh lỗi Unicode
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8")
if hasattr(sys.stderr, "reconfigure"):
    sys.stderr.reconfigure(encoding="utf-8")

# Thêm thư mục gốc Lab04 vào sys.path để nạp cấu hình nếu có
CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
BASE_DIR = os.path.abspath(os.path.join(CURRENT_DIR, "..", ".."))
if BASE_DIR not in sys.path:
    sys.path.insert(0, BASE_DIR)

try:
    from config_loader import get_config
    cfg = get_config()
    SERVER_URL = getattr(cfg, "LOCAL_SERVER_ENDPOINT", "http://127.0.0.1:5000/api/sensor")
except Exception:
    SERVER_URL = "http://127.0.0.1:5000/api/sensor"


def send_to_flask_server(url: str, payload: dict, test_name: str = "Request"):
    """
    Gửi request tới Flask server và hiển thị kết quả chi tiết.
    """
    print(f"\n--- [THỰC HIỆN: {test_name}] ---")
    print(f"[+] Địa chỉ gửi (URL): {url}")
    print("[+] Dữ liệu JSON gửi đi:")
    print(json.dumps(payload, indent=4, ensure_ascii=False))

    try:
        response = requests.post(url, json=payload, timeout=5)
        print(f"\n[+] HTTP Status Code: {response.status_code}")

        # Đọc phản hồi JSON từ Flask Server
        try:
            resp_json = response.json()
            print("[+] JSON Phản hồi từ Server:")
            print(json.dumps(resp_json, indent=4, ensure_ascii=False))
        except ValueError:
            print(f"[+] Phản hồi dạng văn bản: {response.text}")

        if response.status_code == 200:
            print("=> ĐÁNH GIÁ: Request được Server chấp nhận thành công!")
        elif response.status_code == 400:
            print("=> ĐÁNH GIÁ: Server phát hiện dữ liệu không hợp lệ đúng như mong đợi (400)!")
        else:
            print(f"=> ĐÁNH GIÁ: Nhận mã trạng thái {response.status_code}")

        return response

    except requests.exceptions.ConnectionError:
        print("\n[!] LỖI KẾT NỐI: Không thể kết nối tới Flask Server tại " + url)
        print("    -> Nguyên nhân: Bạn chưa khởi động 'server.py'!")
        print("    -> Cách khắc phục:")
        print("       1. Mở một Terminal riêng (Terminal 1) và chạy:")
        print("          python bai_ve_nha/bai2_flask_server/server.py")
        print("       2. Sau đó mở Terminal thứ hai (Terminal 2) chạy lại file này:")
        print("          python bai_ve_nha/bai2_flask_server/client.py")
        return None
    except requests.exceptions.RequestException as e:
        print(f"\n[!] Lỗi khi gửi request: {e}")
        return None


def main():
    print("=" * 68)
    print("       CLIENT KIỂM THỬ KẾT NỐI TỚI FLASK LOCAL SERVER         ")
    print("=" * 68)

    # 1. Kịch bản gửi hợp lệ (Kỳ vọng: 200 OK)
    valid_data = {
        "device_id": "esp32_dht22_node",
        "temperature": 28.5,
        "humidity": 65.0,
        "timestamp": datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    }
    resp1 = send_to_flask_server(SERVER_URL, valid_data, test_name="GỬI DỮ LIỆU HỢP LỆ (200 OK)")

    # 2. Kịch bản gửi thiếu trường 'device_id' (Kỳ vọng: 400 Bad Request)
    if resp1 is not None:
        invalid_data = {
            "temperature": 32.1,
            "humidity": 75.4
            # Cố tình thiếu device_id để kiểm tra validation của server
        }
        send_to_flask_server(SERVER_URL, invalid_data, test_name="THỬ NGHIỆM GỬI THIẾU TRƯỜNG DỮ LIỆU (400 ERROR)")

    print("\n" + "=" * 68)
    print("[*] HOÀN TẤT KIỂM THỬ CLIENT.")
    print("=" * 68)


if __name__ == "__main__":
    main()

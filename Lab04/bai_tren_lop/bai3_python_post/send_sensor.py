# -*- coding: utf-8 -*-
"""
BÀI 3: GỬI HTTP POST REQUEST BẰNG PYTHON (THƯ VIỆN REQUESTS)
-------------------------------------------------------------------------
Chương trình tạo dữ liệu cảm biến dạng dictionary và gửi lên Webhook.site
thông qua hàm requests.post(url, json=data).
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

# Thêm thư mục gốc Lab04 vào sys.path để nạp cấu hình dùng chung
CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
BASE_DIR = os.path.abspath(os.path.join(CURRENT_DIR, "..", ".."))
if BASE_DIR not in sys.path:
    sys.path.insert(0, BASE_DIR)

try:
    from config_loader import get_config
    cfg = get_config()
    WEBHOOK_URL = getattr(cfg, "WEBHOOK_URL", "https://webhook.site/7e245820-4dfb-425a-a195-59e69a15b129")
    DEVICE_ID = getattr(cfg, "DEFAULT_DEVICE_ID", "sensor_01")
except Exception:
    # Nếu chạy riêng lẻ không có module config
    WEBHOOK_URL = "https://webhook.site/7e245820-4dfb-425a-a195-59e69a15b129"
    DEVICE_ID = "sensor_01"


def send_sensor_data(url: str, device_id: str = "sensor_01", timeout: int = 10):
    """
    Tạo dữ liệu cảm biến và gửi POST request tới URL chỉ định.
    """
    print("=" * 65)
    print("          CHƯƠNG TRÌNH GỬI DỮ LIỆU CẢM BIẾN LÊN SERVER          ")
    print("=" * 65)

    # 1. Chuẩn bị dữ liệu cảm biến dạng Python dictionary
    payload = {
        "device_id": device_id,
        "temperature": 27.8,
        "humidity": 65.4,
        "timestamp": datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    }

    # 2. In thông tin trước khi gửi
    print(f"[+] URL đích (Endpoint) : {url}")
    print(f"[+] Dữ liệu gửi đi (JSON):")
    print(json.dumps(payload, indent=4, ensure_ascii=False))
    print("-" * 65)

    try:
        # 3. Gửi HTTP POST request với json=payload và timeout hợp lý
        print("[*] Đang gửi request...")
        response = requests.post(url, json=payload, timeout=timeout)

        # 4. In thông tin kết quả phản hồi từ server
        print(f"[+] HTTP Status Code     : {response.status_code}")
        print(f"[+] Server Response Text : {response.text.strip() if response.text else '(Không có nội dung)'}")

        if response.status_code == 200:
            print("\n=> KẾT QUẢ: Gửi dữ liệu thành công! (200 OK)")
        else:
            print(f"\n=> KẾT QUẢ: Server phản hồi mã {response.status_code}")

        return response

    except requests.exceptions.RequestException as e:
        print(f"\n[!] ĐÃ XẢY RA LỖI KHI GỬI: {e}")
        return None


if __name__ == "__main__":
    # Bạn có thể thay đổi URL trực tiếp tại đây nếu muốn kiểm thử nhanh
    target_url = WEBHOOK_URL
    send_sensor_data(url=target_url, device_id=DEVICE_ID, timeout=10)

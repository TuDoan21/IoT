# -*- coding: utf-8 -*-
"""
BÀI 4: XỬ LÝ XÁC THỰC (AUTHENTICATION/API KEY) TRONG HTTP POST
-------------------------------------------------------------------------
Chương trình gửi dữ liệu cảm biến kèm theo các HTTP Headers xác thực:
- Content-Type: application/json
- X-API-Key: my_secret_key_123
- Authorization: Bearer token_xyz
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
    API_KEY = getattr(cfg, "API_KEY", "my_secret_key_123")
    BEARER_TOKEN = getattr(cfg, "BEARER_TOKEN", "token_xyz")
except Exception:
    WEBHOOK_URL = "https://webhook.site/7e245820-4dfb-425a-a195-59e69a15b129"
    API_KEY = "my_secret_key_123"
    BEARER_TOKEN = "token_xyz"


def send_authenticated_request(url: str, api_key: str, bearer_token: str, timeout: int = 10):
    """
    Gửi POST request kèm custom authentication headers.
    """
    print("=" * 68)
    print("      CHƯƠNG TRÌNH GỬI HTTP POST KÈM HEADER XÁC THỰC (AUTH)       ")
    print("=" * 68)

    # 1. Thiết lập Custom Headers xác thực
    headers = {
        "Content-Type": "application/json",
        "X-API-Key": api_key,
        "Authorization": f"Bearer {bearer_token}"
    }

    # 2. Dữ liệu cảm biến cần gửi
    payload = {
        "device_id": "secure_node_01",
        "temperature": 29.3,
        "humidity": 61.5,
        "status": "authenticated",
        "sent_at": datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    }

    print(f"[+] URL đích (Endpoint) : {url}")
    print("\n[+] Các Headers gửi đi:")
    for k, v in headers.items():
        print(f"    - {k}: {v}")

    print("\n[+] Dữ liệu Body (JSON):")
    print(json.dumps(payload, indent=4, ensure_ascii=False))
    print("-" * 68)

    try:
        print("[*] Đang gửi POST request kèm authentication headers...")
        # Gửi request với tham số headers và json
        response = requests.post(url, headers=headers, json=payload, timeout=timeout)

        print(f"\n[+] Phản hồi HTTP Status Code: {response.status_code}")
        print(f"[+] Nội dung phản hồi         : {response.text.strip() if response.text else '(Rỗng)'}")

        print("\n" + "=" * 68)
        print("HƯỚNG DẪN KIỂM TRA HEADER TRÊN WEBHOOK.SITE:")
        print("1. Mở trang Webhook.site của bạn.")
        print("2. Bấm vào request POST mới nhất vừa nhận được.")
        print("3. Nhìn vào bảng 'Headers' ở giữa màn hình:")
        print(f"   - Kiểm tra xem đã có 'x-api-key: {api_key}' hay chưa.")
        print(f"   - Kiểm tra xem đã có 'authorization: Bearer {bearer_token}' hay chưa.")
        print("4. Chụp lại ảnh màn hình mục Headers này để làm minh chứng Báo cáo.")
        print("=" * 68)

        return response

    except requests.exceptions.RequestException as e:
        print(f"\n[!] Lỗi khi gửi request: {e}")
        return None


if __name__ == "__main__":
    target_url = WEBHOOK_URL

    send_authenticated_request(
        url=target_url,
        api_key=API_KEY,
        bearer_token=BEARER_TOKEN,
        timeout=10
    )

# -*- coding: utf-8 -*-
"""
BÀI 5: XỬ LÝ NGOẠI LỆ (EXCEPTIONS) VÀ TIMEOUT KHI GỬI HTTP REQUEST
-------------------------------------------------------------------------
Chương trình minh họa kỹ thuật lập trình chịu lỗi (Fault-tolerant):
- Thiết lập timeout=3 giây cho HTTP POST request.
- Bắt và xử lý riêng biệt các ngoại lệ phổ biến:
    + requests.exceptions.Timeout: Hết thời gian chờ phản hồi
    + requests.exceptions.ConnectionError: Không thể kết nối tới server
    + requests.exceptions.RequestException: Lỗi chung của thư viện requests
- Chương trình chạy mượt mà, không bị crash (dừng đột ngột) khi gặp sự cố.
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

# Thêm thư mục gốc Lab04 vào sys.path để nạp cấu hình
CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
BASE_DIR = os.path.abspath(os.path.join(CURRENT_DIR, "..", ".."))
if BASE_DIR not in sys.path:
    sys.path.insert(0, BASE_DIR)

try:
    from config_loader import get_config
    cfg = get_config()
    WEBHOOK_URL = getattr(cfg, "WEBHOOK_URL", "https://webhook.site/7e245820-4dfb-425a-a195-59e69a15b129")
except Exception:
    WEBHOOK_URL = "https://webhook.site/7e245820-4dfb-425a-a195-59e69a15b129"


def send_request_with_handling(url: str, timeout: float = 3.0):
    """
    Gửi dữ liệu cảm biến kèm cơ chế bắt lỗi ngoại lệ và timeout chặt chẽ.
    """
    print("-" * 68)
    print(f"[*] Đang chuẩn bị gửi request tới: {url}")
    print(f"[*] Cấu hình Timeout: {timeout} giây")

    payload = {
        "device_id": "resilient_node_05",
        "temperature": 26.5,
        "humidity": 70.0,
        "test_time": datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    }

    try:
        # Gửi request với timeout=3 giây
        response = requests.post(url, json=payload, timeout=timeout)

        # Nếu thành công
        print(f"[+] THÀNH CÔNG! Mã phản hồi HTTP: {response.status_code}")
        print(f"[+] Dữ liệu server phản hồi: {response.text[:150]}...")
        return True

    except requests.exceptions.Timeout as err_timeout:
        # 1. Bắt riêng lỗi hết thời gian chờ
        print("[!] [LỖI TIMEOUT] Quá thời gian chờ phản hồi từ server!")
        print(f"    -> Chi tiết: Server không phản hồi trong vòng {timeout} giây.")
        print(f"    -> Nguyên nhân thường gặp: Mạng chập chờn, server bị quá tải hoặc xử lý quá lâu.")
        print(f"    -> Log kỹ thuật: {err_timeout}")
        return False

    except requests.exceptions.ConnectionError as err_conn:
        # 2. Bắt riêng lỗi kết nối không thành công
        print("[!] [LỖI KẾT NỐI - CONNECTION ERROR] Không thể thiết lập kết nối tới server!")
        print(f"    -> Chi tiết: Địa chỉ IP/Tên miền không tồn tại, cổng đóng hoặc chưa bật server.")
        print(f"    -> Nguyên nhân: Sai URL, server mục tiêu đang tắt hoặc mất kết nối mạng.")
        print(f"    -> Log kỹ thuật: {err_conn}")
        return False

    except requests.exceptions.RequestException as err_req:
        # 3. Bắt các lỗi chung khác của requests (Invalid URL, HTTPError, v.v.)
        print("[!] [LỖI REQUEST TỔNG QUÁT] Đã xảy ra lỗi trong quá trình thực hiện request.")
        print(f"    -> Log kỹ thuật: {err_req}")
        return False

    except Exception as e:
        # 4. Dự phòng các lỗi ngoại lệ hệ thống khác
        print(f"[!] [LỖI NGOẠI LỆ BẤT THƯỜNG] {e}")
        return False


def run_demonstration():
    """
    Chạy thử nghiệm 3 kịch bản thực tế để sinh viên quan sát cơ chế xử lý lỗi.
    """
    print("=" * 68)
    print("      KIỂM TRA CÁC KỊCH BẢN XỬ LÝ NGOẠI LỆ VÀ TIMEOUT (LAB 4 - BÀI 5) ")
    print("=" * 68)
    print("Chương trình sẽ tự động mô phỏng 3 kịch bản mà KHÔNG CẦN tắt mạng internet:\n")

    # Kịch bản 1: URL hợp lệ (Webhook.site hoặc test endpoint)
    print("\n>>> KỊCH BẢN 1: Gửi tới URL hợp lệ (Kỳ vọng: Thành công)")
    valid_url = WEBHOOK_URL
    send_request_with_handling(valid_url, timeout=3.0)

    # Kịch bản 2: Gây lỗi Timeout bằng endpoint trễ 5 giây (với timeout=3s)
    print("\n>>> KỊCH BẢN 2: Mô phỏng lỗi TIMEOUT (Kỳ vọng: Bắt lỗi Timeout sau 3 giây)")
    # httpbin.org/delay/5 sẽ đợi 5 giây mới phản hồi, trong khi timeout chỉ cho phép 3 giây
    delay_url = "https://httpbin.org/delay/5"
    send_request_with_handling(delay_url, timeout=3.0)

    # Kịch bản 3: Gây lỗi ConnectionError bằng cổng localhost không hoạt động
    print("\n>>> KỊCH BẢN 3: Mô phỏng lỗi CONNECTION ERROR (Kỳ vọng: Bắt lỗi kết nối)")
    # Cổng 59999 trên máy local chắc chắn không có service nào chạy
    invalid_conn_url = "http://127.0.0.1:59999/api/nonexistent"
    send_request_with_handling(invalid_conn_url, timeout=3.0)

    print("\n" + "=" * 68)
    print("[*] HOÀN TẤT THỬ NGHIỆM!")
    print("    Nhận xét: Chương trình đã xử lý an toàn tất cả các lỗi ngoại lệ,")
    print("    không có bất kỳ màn hình crash (Traceback lỗi đỏ) nào làm gián đoạn hệ thống.")
    print("=" * 68)


if __name__ == "__main__":
    run_demonstration()

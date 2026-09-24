# -*- coding: utf-8 -*-
"""
BÀI VỀ NHÀ 1: CHƯƠNG TRÌNH GỬI DỮ LIỆU CẢM BIẾN ĐỊNH KỲ (PERIODIC SENDER)
-------------------------------------------------------------------------
Mô phỏng một trạm quan trắc IoT:
- Tự động sinh dữ liệu ngẫu nhiên:
    + Nhiệt độ: 20.0°C - 40.0°C
    + Độ ẩm: 50.0% - 90.0%
    + Timestamp & Device ID
- Gửi HTTP POST lên Webhook.site định kỳ mỗi 10 giây.
- Sử dụng time.sleep(10) tiết kiệm CPU tối đa.
- Bắt lỗi mạng/timeout để duy trì vòng lặp không bị chết.
- Cho phép dừng an toàn bằng phím Ctrl + C.
-------------------------------------------------------------------------
"""

import os
import sys
import time
import json
import random
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
    DEVICE_ID = getattr(cfg, "DEFAULT_DEVICE_ID", "iot_station_01")
except Exception:
    WEBHOOK_URL = "https://webhook.site/7e245820-4dfb-425a-a195-59e69a15b129"
    DEVICE_ID = "iot_station_01"

# Khoảng thời gian định kỳ giữa các lần gửi (giây)
SEND_INTERVAL_SECONDS = 10


def generate_sensor_data(device_id: str) -> dict:
    """
    Sinh dữ liệu cảm biến ngẫu nhiên theo dải yêu cầu:
    - Nhiệt độ: 20°C - 40°C
    - Độ ẩm: 50% - 90%
    """
    temperature = round(random.uniform(20.0, 40.0), 2)
    humidity = round(random.uniform(50.0, 90.0), 2)
    current_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    return {
        "device_id": device_id,
        "temperature": temperature,
        "humidity": humidity,
        "timestamp": current_time
    }


def run_periodic_sender(url: str, device_id: str, interval: int = 10):
    """
    Vòng lặp gửi dữ liệu định kỳ mỗi interval giây.
    """
    print("=" * 70)
    print("      CHƯƠNG TRÌNH GỬI DỮ LIỆU CẢM BIẾN ĐỊNH KỲ (MỖI 10 GIÂY)        ")
    print("=" * 70)
    print(f"[*] URL đích        : {url}")
    print(f"[*] Mã thiết bị     : {device_id}")
    print(f"[*] Chu kỳ gửi      : {interval} giây/lần")
    print("[*] Hướng dẫn dừng  : Nhấn tổ hợp phím [Ctrl + C] để dừng chương trình.")
    print("=" * 70)

    sequence = 1

    try:
        while True:
            # 1. Sinh dữ liệu ngẫu nhiên
            sensor_payload = generate_sensor_data(device_id)

            print(f"\n[LẦN GỬI #{sequence}] Thời điểm: {sensor_payload['timestamp']}")
            print(f"    -> Dữ liệu gửi: Temp = {sensor_payload['temperature']}°C | "
                  f"Humidity = {sensor_payload['humidity']}% | Device = {sensor_payload['device_id']}")

            # 2. Gửi dữ liệu và xử lý lỗi mạng
            try:
                response = requests.post(url, json=sensor_payload, timeout=5)
                print(f"    -> Trạng thái phản hồi: HTTP {response.status_code}")
                if response.status_code == 200:
                    print("    -> Kết quả: Gửi thành công lên server!")
                else:
                    print(f"    -> Kết quả: Server phản hồi mã {response.status_code}")

            except requests.exceptions.Timeout:
                print("    -> [CẢNH BÁO] Quá thời gian chờ (Timeout). Vẫn duy trì chương trình...")
            except requests.exceptions.ConnectionError:
                print("    -> [CẢNH BÁO] Lỗi kết nối (Connection Error). Sẽ thử lại ở chu kỳ kế tiếp...")
            except requests.exceptions.RequestException as req_err:
                print(f"    -> [CẢNH BÁO] Lỗi request: {req_err}")

            # 3. Nghỉ đúng 10 giây (tiết kiệm CPU, không tốn tài nguyên)
            print(f"[*] Đang chờ {interval} giây cho chu kỳ tiếp theo...")
            time.sleep(interval)
            sequence += 1

    except KeyboardInterrupt:
        # Bắt phím Ctrl + C để dừng an toàn
        print("\n\n" + "=" * 70)
        print("[*] NHẬN ĐƯỢC TÍN HIỆU NGẮT (Ctrl + C) TỪ BÀN PHÍM!")
        print(f"[*] Đã dừng an toàn chương trình sau {sequence - 1} lần gửi dữ liệu.")
        print("[*] Cảm ơn bạn đã sử dụng chương trình. Hẹn gặp lại!")
        print("=" * 70)


if __name__ == "__main__":
    run_periodic_sender(
        url=WEBHOOK_URL,
        device_id=DEVICE_ID,
        interval=SEND_INTERVAL_SECONDS
    )

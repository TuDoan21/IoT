# -*- coding: utf-8 -*-
"""
Bộ nạp cấu hình dùng chung cho toàn bộ bài thực hành Lab 04.
Tự động ưu tiên tải 'config.py' (nếu có), nếu chưa có sẽ nạp 'config.example.py'.
"""

import os
import sys
import importlib.util
from types import SimpleNamespace

# Tự động cấu hình chuẩn UTF-8 cho console Windows tránh lỗi Unicode
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8")
if hasattr(sys.stderr, "reconfigure"):
    sys.stderr.reconfigure(encoding="utf-8")

# Đường dẫn thư mục gốc Lab04
BASE_DIR = os.path.dirname(os.path.abspath(__file__))


def load_config_file(filepath):
    """Nạp file python thành module một cách an toàn"""
    module_name = os.path.splitext(os.path.basename(filepath))[0]
    spec = importlib.util.spec_from_file_location(module_name, filepath)
    if spec and spec.loader:
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        return module
    return None


def get_config():
    """
    Trả về cấu hình. Ưu tiên config.py, nếu không có fallback về config.example.py.
    """
    config_real = os.path.join(BASE_DIR, "config.py")
    config_example = os.path.join(BASE_DIR, "config.example.py")

    if os.path.exists(config_real):
        cfg = load_config_file(config_real)
        if cfg:
            return cfg

    # Fallback về config.example.py
    if os.path.exists(config_example):
        cfg = load_config_file(config_example)
        if cfg:
            print("=" * 70)
            print("[!] THÔNG BÁO: Đang nạp cấu hình từ 'config.example.py'.")
            print("    Để cấu hình URL Webhook cá nhân của bạn, hãy tạo bản sao:")
            print("    > copy config.example.py config.py")
            print("    Sau đó sửa giá trị WEBHOOK_URL trong 'config.py'.")
            print("=" * 70)
            return cfg

    # Trường hợp dự phòng tối hậu
    fallback = SimpleNamespace(
        WEBHOOK_URL="https://webhook.site/7e245820-4dfb-425a-a195-59e69a15b129",
        DEFAULT_DEVICE_ID="sensor_esp32_01",
        API_KEY="my_secret_key_123",
        BEARER_TOKEN="token_xyz",
        LOCAL_SERVER_HOST="127.0.0.1",
        LOCAL_SERVER_PORT=5000,
        LOCAL_SERVER_ENDPOINT="http://127.0.0.1:5000/api/sensor"
    )
    return fallback

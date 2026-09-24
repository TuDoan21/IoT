# -*- coding: utf-8 -*-
"""
FILE CẤU HÌNH MẪU CHO LAB 04 - INTERNET OF THINGS (IoT)
-------------------------------------------------------------------------
HƯỚNG DẪN SỬ DỤNG:
1. Tạo một bản sao của file này và đặt tên là 'config.py':
   (Lệnh Windows: copy config.example.py config.py)
2. Mở file 'config.py' vừa tạo, thay đổi WEBHOOK_URL thành URL cá nhân
   lấy từ trang web: https://webhook.site
3. File 'config.py' đã được khai báo trong .gitignore nên sẽ không bị commit
   lên Git, giúp bảo mật API Key và Webhook URL cá nhân của bạn.
-------------------------------------------------------------------------
"""

# =========================================================================
# 1. CẤU HÌNH WEBHOOK.SITE (Dùng cho Bài 3, 4, 5 trên lớp và Bài 1 về nhà)
# =========================================================================
# URL thực tế trên https://webhook.site của bạn
WEBHOOK_URL = "https://webhook.site/7e245820-4dfb-425a-a195-59e69a15b129"

# Mã định danh thiết bị IoT mặc định
DEFAULT_DEVICE_ID = "sensor_esp32_01"

# =========================================================================
# 2. CẤU HÌNH XÁC THỰC - AUTHENTICATION (Dùng cho Bài 4 trên lớp)
# =========================================================================
# Đây là các token/key minh họa phục vụ học tập, không dùng khóa thật
API_KEY = "my_secret_key_123"
BEARER_TOKEN = "token_xyz"

# =========================================================================
# 3. CẤU HÌNH LOCAL FLASK SERVER (Dùng cho Bài 2 về nhà)
# =========================================================================
LOCAL_SERVER_HOST = "127.0.0.1"
LOCAL_SERVER_PORT = 5000
LOCAL_SERVER_ENDPOINT = f"http://{LOCAL_SERVER_HOST}:{LOCAL_SERVER_PORT}/api/sensor"

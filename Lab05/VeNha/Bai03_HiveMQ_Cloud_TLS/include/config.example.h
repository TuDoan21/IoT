#ifndef CONFIG_H
#define CONFIG_H

// ==========================================
// CẤU HÌNH WI-FI
// ==========================================
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// ==========================================
// CẤU HÌNH HIVEMQ CLOUD (TLS)
// ==========================================
// Thay bằng URL Cluster của bạn trên HiveMQ Cloud (ví dụ: "xxxxxx.s1.eu.hivemq.cloud")
#define MQTT_BROKER "YOUR_CLUSTER_URL.hivemq.cloud"
// Cổng TLS của HiveMQ Cloud (mặc định luôn là 8883, KHÔNG hỗ trợ 1883)
#define MQTT_PORT 8883

// Thông tin xác thực do bạn tạo trên Console HiveMQ Cloud (Tab "Access Management")
#define MQTT_USERNAME "YOUR_HIVEMQ_USERNAME"
#define MQTT_PASSWORD "YOUR_HIVEMQ_PASSWORD"

// Client ID duy nhất cho kết nối Cloud
#define MQTT_CLIENT_ID "esp32-doantuantu-cloud"

// Tiền tố topic cá nhân và topic kiểm thử
#define TOPIC_PREFIX "lab05/doantuantu"
#define TOPIC_CLOUD_STATUS "lab05/doantuantu/cloud/status"

// ==========================================
// CHỨNG CHỈ GỐC (ROOT CA CERTIFICATE)
// HiveMQ Cloud sử dụng chứng chỉ SSL được ký bởi Let's Encrypt (ISRG Root X1).
// Chúng ta xác thực server bằng chứng chỉ gốc này thay vì dùng setInsecure().
// ==========================================
static const char HIVEMQ_ROOT_CA[] PROGMEM = 
"-----BEGIN CERTIFICATE-----\n"
"MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n"
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n"
"WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n"
"ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n"
"MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n"
"h77ct984kIffPoGfBQVQoc26n+/HVT2cU5zZmTeu5hDj66W3zy3UVWtBQ5qspcjD\n"
"3xWQykYESRxptnlspHV+SxKEpmhnNeBDqhn5dVWgQKWhFoeYzWB3zL01AKdX0ki3\n"
"YR6TN3yNzR258N475bKa1S4wgNQBV3769v00NgxigZLSfZxKm26xWIEQPHJ2WEA5\n"
"nQCdLeeK3CL3F9QVCDZ479PXeKSMUfYzs2C6Yq7P/kslbmQi828N6+QKYuzfGgnm\n"
"Ks22PELogVW8SXd9Kp/A07BAQCnK1JnZOtUCn9WKntStESGKnQn/stvtgDL639UK\n"
"sbKW04BDT+1358XSETJFtx4BXgiT+/ETbqbxZ6ONdCWUBQAZtJysapJY8PXKeySp\n"
"36dP0LMucjAETyg52108Y0aoGWqh0LS0Gyalo0md3URHZ256mKMrGDQtTD1VCCDW\n"
"2R+02gh54j9j4IUj7c42L3a4VDfzGyh3ExZCJFsP5542LspQShGQeTbOTK4xXdCow\n"
"vGTRtUCvdhMpRNbqY036aNYx0xbr2VME64bfFiLnhR83uh+7vcLB9l2cnGQ9190a\n"
"hFx8bC88KcJHwn1DYGQBAEp17N23UUMtwSWJFJa+5TubNm0TCTXqqByVDQKTMItg\n"
"6ZMvdXNetT5G4N2Z97dB4TQFMdeGdQIDAQABo0IwADAOBgNVHQ8BAf8EBAMCAQYw\n"
"DwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQU5KNuTRrkmsjhNWhRqImemlzkNIgw\n"
"DQYJKoZIhvcNAQELBQADggIBAKMLstikPWLGMUNghYsKL+xYgCFQQ/Fa0VtREhEi\n"
"431560MdV6390hNuCXgkTQybwGWGi/8RURciiWH40GpMtFVxTXTvYbVUrmQLwdT3\n"
"XDHMvd1SWlQieNAIEZDMAEAPsTKaQGoNZ5JlFst90xSkTWwrDxnyDTqhP2GS07Wz\n"
"BqeeaCfgCGfcHQdaKUVyDZmaAnjPOKBQo/H000ajUHB+I7KEk6NElDOPHudxm88s\n"
"mx38nTW5+/eyuiqUMecq39lZK1HsackYvY+hA01P/88Nonpybc43mWnjnbRLvT2k\n"
"mDTwxNZefRg8DmYTdtuPkkWDZZZZFoWd1GLTQbpcMLBaZMhFAPVQTWDn/SL6EC29\n"
"ktI7/PxKFcxNdT807hKSnFhf7+cvNnaAFe95xUeaVDPj8KA88jIb3mxnH477R60W\n"
"A8Z9g8NRo50i2bZ9HUwHWwzYg5bUT5IyFA2qaICUQC+WlO790U979QIDAQAB\n"
"-----END CERTIFICATE-----\n";

#endif // CONFIG_H

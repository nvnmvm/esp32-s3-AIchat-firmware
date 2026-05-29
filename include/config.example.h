#pragma once

// Copy this file to include/config.h, then edit values before flashing.
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// Phase 1 uses plain WebSocket echo: ws://YOUR_VPS_IP:PORT/ws?token=...
#define WS_HOST "YOUR_VPS_IP_OR_DOMAIN"
#define WS_PORT 8000
#define WS_PATH "/ws"
#define WS_USE_SSL false
#define WS_TOKEN "CHANGE_ME_TO_THE_CLOUD_TOKEN"

#define DEVICE_ID "esp32-s3-voice-001"
#define HELLO_INTERVAL_MS 1000

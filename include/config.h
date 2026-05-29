#pragma once

// Edit these values before flashing.
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// Phase 1 uses plain WebSocket echo: ws://YOUR_VPS_IP:8000/ws?token=...
#define WS_HOST "YOUR_VPS_IP_OR_DOMAIN"
#define WS_PORT 8000
#define WS_PATH "/ws"
#define WS_USE_SSL false
#define WS_TOKEN "CHANGE_ME_TO_THE_CLOUD_TOKEN"

// Reserved for the next AI phase. Phase 1 does not send or log this value.
#define AI_API_KEY "YOUR_AI_API_KEY"

#define DEVICE_ID "esp32-s3-voice-001"
#define HELLO_INTERVAL_MS 1000

#pragma once

// Copy this file to include/config.h, then edit values before flashing.
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// Phase 3 keeps the JSON + PCM WebSocket protocol:
// ws://YOUR_VPS_IP:PORT/ws?token=...
#define WS_HOST "YOUR_VPS_IP_OR_DOMAIN"
#define WS_PORT 8000
#define WS_PATH "/ws"
#define WS_USE_SSL false
#define WS_TOKEN "CHANGE_ME_TO_THE_CLOUD_TOKEN"

#define DEVICE_ID "esp32-s3-voice-001"

// Audio protocol: PCM signed 16-bit little-endian, 16 kHz, mono.
#define AUDIO_SAMPLE_RATE 16000
#define AUDIO_CHUNK_MS 40
#define RECORD_MAX_MS 8000

// OLED: SH1106, U8g2, software I2C, 0x3C, SDA GPIO8, SCL GPIO9.
#define OLED_ENABLED true

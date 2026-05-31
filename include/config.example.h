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
#define RECORD_MIN_MS 900
#define RECORD_MAX_MS 12000
#define RECORD_SEND_AFTER_FINISH_GUARD_MS 120
#define AUDIO_STATS_INTERVAL_MS 500
#define SEND_AUDIO_STATS_TO_CLOUD true

// Audio capture quality. Some I2S microphones need the right channel instead.
#define MIC_CHANNEL_LEFT true
#define MIC_GAIN_SHIFT 0
#define MIC_INVERT_SIGNAL false

// Cloud protocol compatibility.
#define CLOUD_PROTOCOL_VERSION 301
#define SEND_START_RECORD_METADATA true

// OLED: SH1106, U8g2, software I2C, 0x3C, SDA GPIO8, SCL GPIO9.
#define OLED_ENABLED true

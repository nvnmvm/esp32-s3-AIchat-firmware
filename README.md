# ESP32-S3 Firmware

Phase 1 firmware for testing WiFi plus WebSocket echo with the cloud service.

## Clone From GitHub

After this firmware project is pushed to GitHub:

```bash
git clone https://github.com/nvnmvm/esp32-s3-AIchat-firmware.git
cd esp32-s3-AIchat-firmware
```

## Configure

Edit `include/config.h`:

- `WIFI_SSID`
- `WIFI_PASSWORD`
- `WS_HOST`
- `WS_PORT`
- `WS_TOKEN`
- `AI_API_KEY` is reserved for the next AI phase and is not used in phase 1.

## Flash

Install PlatformIO first, then run:

```powershell
cd esp32-s3-firmware
.\scripts\flash.ps1
```

Or run PlatformIO directly:

```bash
pio run -t upload
pio device monitor
```

Expected serial output:

- `WiFi connected`
- `WebSocket connected`
- `Sent: hello from esp32 ...`
- `WebSocket text received ...`

# ESP32-S3 AI 对话机器人固件

当前版本：`v4.0.0-realtime-foundation`，配套云端协议：`400`。

该版本加入 Qwen 实时 ASR 对应的协议消息、OLED 局部识别字幕、服务端 VAD、turn ID、防旧消息覆盖、异步 I2S 播放队列和播放中语音打断。云端和固件仍按半双工工作，不宣称已经具备无 AEC 的真全双工能力。

## 快速开始

```bash
bash scripts/init-config.sh
```

编辑 `include/config.h`：

```cpp
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define WS_HOST "voice.example.com"
#define WS_PORT 443
#define WS_USE_SSL true
#define WS_TOKEN "与云端一致的token"
#define CLOUD_PROTOCOL_VERSION 400
```

构建和烧录：

```bash
pio run
pio run -t upload
pio device monitor -b 115200
```

## 协议摘要

连接后先发送：

```json
{"type":"hello","protocol":400,"firmware":"v4.0.0-realtime-foundation","device_id":"esp32-s3-voice-001"}
```

唤醒后发送 `turn_start`，随后持续上传 16 kHz/mono/s16le PCM。收到 `capture_stop` 后发送 `turn_end`。收到 `asr_partial` 时 OLED 更新字幕。播放或处理中再次唤醒会发送 `cancel(reason=barge_in)`、清空旧音频并开始新 turn。

## 文档

- [阶段四详细开发、配置和验收文档](docs/README-phase-4.md)
- [阶段四发布说明](RELEASE-v4.0.0-realtime-foundation.md)
- [硬件引脚](include/pins.h)
- [配置模板](include/config.example.h)

`include/config.h` 包含 Wi-Fi 和 token，不要提交到 Git。

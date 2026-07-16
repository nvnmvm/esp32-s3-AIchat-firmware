# ESP32-S3 AI 对话机器人固件

当前版本：`v4.1.0-streaming-pipeline`，配套云端协议：`400`。

该版本在 Qwen 实时 ASR、turn ID、异步 I2S 播放和语音打断基础上，加入 `answer_delta` 增量回答显示、流式回答状态和最终文本/播放状态兼容处理。云端和固件仍按半双工工作，不宣称已经具备无 AEC 的真全双工能力。

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
{"type":"hello","protocol":400,"firmware":"v4.1.0-streaming-pipeline","device_id":"esp32-s3-voice-001"}
```

唤醒后发送 `turn_start`，随后持续上传 16 kHz/mono/s16le PCM。收到 `capture_stop` 后发送 `turn_end`。`asr_partial` 更新识别字幕，`answer_delta` 逐步追加回答，第一段 `audio_start` 到达后立即播放。播放或处理中再次唤醒会发送 `cancel(reason=barge_in)`、清空旧音频并开始新 turn。

## 文档

- [阶段四详细开发、配置和验收文档](docs/README-phase-4.md)
- [阶段 4.1 发布说明](RELEASE-v4.1.0-streaming-pipeline.md)
- [阶段四基础版发布说明](RELEASE-v4.0.0-realtime-foundation.md)
- [硬件引脚](include/pins.h)
- [配置模板](include/config.example.h)

`include/config.h` 包含 Wi-Fi 和 token，不要提交到 Git。

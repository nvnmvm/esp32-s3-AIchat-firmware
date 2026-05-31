# ESP32-S3 AI 对话机器人固件

当前版本：`v3.0.1-phase3-asr-quality`。

本固件配套云端 `v3.0.1-phase3-asr-quality`，保持 JSON + PCM WebSocket 协议，同时补充录音质量统计、录音边界保护和 OLED 显示流转修正。

## 3.0.1 变化

- `start_record` 可携带协议、音频格式、设备 ID、麦克风声道和固件版本 metadata。
- 录音时统计 `bytes`、`chunks`、`rms`、`peak`、`clipped`，通过 `audio_stats` 发给云端。
- 增加 `MIC_CHANNEL_LEFT`、`MIC_GAIN_SHIFT`、`MIC_INVERT_SIGNAL`，方便排查声道反了、音量过低、信号反相等问题。
- OLED 不再显示识别结果页，也不显示回答总览页；收到音频播放开始后直接显示滚动回答。
- 回答播放/滚动结束附近的正常 WebSocket 重连不再强制显示“云端断开”。
- WebSocket 心跳放宽，减少 TTS 播放期间误判断连。

## 配置

复制并编辑：

```bash
cp include/config.example.h include/config.h
```

关键配置：

```cpp
#define WS_HOST "YOUR_VPS_IP_OR_DOMAIN"
#define WS_PORT 8000
#define WS_TOKEN "CHANGE_ME_TO_THE_CLOUD_TOKEN"

#define MIC_CHANNEL_LEFT true
#define MIC_GAIN_SHIFT 0
#define MIC_INVERT_SIGNAL false
#define RECORD_MIN_MS 900
#define RECORD_MAX_MS 12000
#define SEND_AUDIO_STATS_TO_CLOUD true
#define CLOUD_PROTOCOL_VERSION 301
```

如果云端 `audio_report.json` 显示 `mostly_zero` 或 `too_quiet`，优先尝试：

1. 切换 `MIC_CHANNEL_LEFT`。
2. 检查 I2S 麦克风接线和供电。
3. 适当调整 `MIC_GAIN_SHIFT`，同时观察 `clipped` 是否升高。

## 构建

```bash
pio run
```

烧录：

```bash
pio run -t upload
```

或使用脚本：

```bash
bash scripts/flash.sh
```

## 协议

ESP32 到云端：

```json
{"type":"start_record","protocol":301,"audio":{"format":"pcm_s16le","sample_rate":16000,"channels":1,"chunk_ms":40},"device":{"id":"esp32-s3-voice-001","mic_channel":"left","firmware":"v3.0.1-phase3-asr-quality"}}
```

之后持续发送 PCM 二进制块，并周期性发送：

```json
{"type":"audio_stats","reason":"recording","bytes":32000,"chunks":25,"rms":1200,"peak":8000,"clipped":0,"mic_channel":"left"}
```

云端完成 VAD 或固件达到最大录音时间后进入处理和播放。

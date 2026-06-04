# ESP32-S3 AI 对话机器人固件

当前版本：`v3.0.3-config-readiness`。

本固件配套云端 `v3.0.3-config-readiness`，保持 JSON + PCM WebSocket 协议。3.0.3 云端重点修正部署与模型配置路径，固件侧同步协议 metadata 和版本号，继续沿用 3.0.2 的录音质量统计、录音边界保护和 OLED 显示流转修正。

## 3.0.3 变化

- `start_record` metadata 中的 `firmware` 更新为 `v3.0.3-config-readiness`。
- `CLOUD_PROTOCOL_VERSION` 默认更新为 `303`，用于云端日志和排查时区分固件版本。
- 配套云端 3.0.3 的无模型测试路径：即使暂时不配置 ASR/LLM API，也可以先验证录音、OLED、喇叭和 WebSocket 链路。
- OLED 仍不显示识别结果页，也不显示回答总览页；收到回答文本后直接进入滚动回复页面。
- 回答播放 / 滚动结束附近的正常 WebSocket 重连不会强制显示“云端断开”页面。
- 音频统计、VAD 边界、杂散音频兼容逻辑保持 3.0.2 行为。

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
#define CLOUD_PROTOCOL_VERSION 303
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
{"type":"start_record","protocol":303,"audio":{"format":"pcm_s16le","sample_rate":16000,"channels":1,"chunk_ms":40},"device":{"id":"esp32-s3-voice-001","mic_channel":"left","firmware":"v3.0.3-config-readiness"}}
```

之后持续发送 PCM 二进制块，并周期性发送：

```json
{"type":"audio_stats","reason":"recording","bytes":32000,"chunks":25,"rms":1200,"peak":8000,"clipped":0,"mic_channel":"left"}
```

云端完成 VAD 或固件达到最大录音时间后进入处理和播放。

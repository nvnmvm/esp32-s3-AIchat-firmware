# v3.0.0-phase3-session-voice

阶段三 session 语音助手配套固件版。本版本继续沿用阶段二 JSON + PCM WebSocket 协议，配合云端 `v3.0.0-phase3-session-voice` 使用。

## 主要变化

- 启动串口和 OLED 初始提示更新为阶段三。
- 保留 ASRPRO 唤醒、I2S 麦克风上传、OLED 回答横向滚动和 MAX98357A PCM 播放。
- 兼容云端返回的 `asr_text`、`answer_text`、`audio_start`、二进制 PCM 和 `audio_end`。
- 云端新增的 `transcript_file`、`answer_file`、`answer_chars`、`truncated` 字段会被 ArduinoJson 忽略，不影响旧显示流程。

## 使用说明

1. 先部署云端阶段三。
2. 复制 `include/config.example.h` 为 `include/config.h`。
3. 填入 Wi-Fi、VPS IP、端口和 WebSocket token。
4. 使用 PlatformIO 编译烧录。

```bash
pio run -t upload
pio device monitor
```

唤醒后，ESP32 会上传录音，OLED 显示识别结果和 AI 回答滚动文本，并播放云端 TTS PCM。

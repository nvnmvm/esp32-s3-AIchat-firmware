# v3.0.3-config-readiness

本固件版本配套云端 `v3.0.3-config-readiness`。

## 固件变化

- `start_record` metadata 中的 `firmware` 更新为 `v3.0.3-config-readiness`。
- `CLOUD_PROTOCOL_VERSION` 默认更新为 `303`，便于云端日志和录音诊断区分版本。
- 保持 JSON + PCM WebSocket 协议，继续使用 16 kHz、16-bit、mono、40 ms PCM chunk。
- OLED 仍不显示 ASR 识别结果页和回答总览页，直接进入滚动回答页面。
- 保持 3.0.2 的音频统计、录音边界保护和正常重连不误报云端断开的行为。

## 配套云端

云端 3.0.3 支持无模型测试路径。用户可以先烧录固件并验证录音、OLED、喇叭和 WebSocket 链路，再进入云端 `manage.sh > 模型与语音 > 首次配置向导` 配置 ASR/LLM。

## 构建

```bash
pio run
```

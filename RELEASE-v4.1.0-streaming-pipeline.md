# v4.1.0-streaming-pipeline

阶段 4.1 固件适配云端 SSE LLM 与分句 TTS，同时保持协议 400 和 v4.0 音频播放兼容。

## 主要变化

- 处理 `answer_delta`，在完整回答生成前逐步追加 OLED 文本。
- 新增“回答生成中”显示状态；`audio_start` 后复用已累计文本进入滚动显示。
- 播放中收到最终 `answer_text` 时只更新文本，不再错误改为 `Processing`，也不提前设置音频完成。
- 新 turn 开始时清空旧回答，避免跨轮拼接。
- 回答缓存限制为 4096 UTF-8 字节，并在 UTF-8 字符边界截断。
- 固件版本字符串集中为 `FIRMWARE_VERSION`，删除会强制使用已弃用 I2S 枚举的错误兼容宏。

## 兼容性

- 协议号保持 `400`，配套云端版本为 `v4.1.0-streaming-pipeline`。
- 云端不发送 `answer_delta` 时，固件仍按 `answer_text` + `audio_start` 工作。
- 当前仍是可打断半双工；扬声器播放期间不持续上传麦克风，不需要虚假宣称 AEC/全双工。

## 验证

```bash
pio run
```

默认 `esp32-s3-devkitc-1` N8、无 PSRAM环境应编译成功。烧录后还需验证增量文本、两句以上连续播放、播放中最终文本、barge-in、断网重连和至少 50 轮稳定性。

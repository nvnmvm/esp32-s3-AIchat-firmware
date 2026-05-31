# v3.0.1-phase3-asr-quality

## 固件

- 新增 I2S 麦克风声道、增益、反相配置。
- 新增本轮录音 `rms/peak/clipped/bytes/chunks` 统计并发送 `audio_stats`。
- `start_record` 增加 3.0.1 metadata。
- 隐藏 OLED 识别结果页和回答总览页，直接进入滚动回答。
- 回答完成附近的正常 WebSocket 重连不再显示“云端断开”。
- WebSocket heartbeat 调整为更宽松的 30s / 10s / 3 次。

## 配套云端

要求云端 `v3.0.1-phase3-asr-quality` 或兼容版本。

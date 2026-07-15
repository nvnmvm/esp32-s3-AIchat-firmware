# v4.0.0-realtime-foundation

阶段四固件首个可发布增量。

## 主要变化

- 协议号更新为 400，并加入 hello、turn ID、局部 ASR、服务端 VAD 和取消事件。
- I2S 播放从 WebSocket 回调迁移到 FreeRTOS 有界队列任务。
- 支持在处理/播放过程中再次唤醒并打断旧轮。
- 移除未使用的状态和配置。

## 验证

- `pio run` 在 `esp32-s3-devkitc-1` 环境编译成功。
- 默认 N8 无 PSRAM目标的静态 RAM 和 Flash 占用保持在可用范围。
- 配套云端版本：`v4.0.0-realtime-foundation`。

详细配置、协议和硬件验收步骤见 `docs/README-phase-4.md`。

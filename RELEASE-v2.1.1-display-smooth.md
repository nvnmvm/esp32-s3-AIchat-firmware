# v2.1.1-display-smooth

阶段二显示闭环完善版。本版本只改固件，不改云端协议，目标是在阶段二完成稳定的 AI 对话核心体验：唤醒、录音、云端回答、OLED 显示、MAX98357A 播放。

## 重点变化

- 新增 OLED 显示状态机：WebSocket 事件只修改显示状态，`loop()` 统一刷新 OLED。
- 新增回答横向滚动：AI 回答文字从右向左滚动显示。
- `audio_start` 不再显示“正在播放”，不会占用或覆盖回答正文。
- `audio_end` 只标记音频结束，必须等文字滚动也结束后才显示 `回答完毕`。
- `回答完毕` 保留约 2 秒，然后回到 `等待唤醒` 和上海时间。
- OLED 刷新间隔为 40ms，约 25 FPS，适配 SH1106 + U8g2 软件 I2C。

## 阶段二边界

本版本不引入阶段三能力：

- 不做音频环形缓冲。
- 不做真正边收边播调度。
- 不做 ASR partial 实时字幕。
- 不做 DeepSeek 流式文字同步。

这些会留到阶段三实时流式对话版本实现。

## 配套版本

- 云端推荐版本：[v2.1.2-phase2-complete](https://github.com/nvnmvm/esp32-s3-AIchat/releases/tag/v2.1.2-phase2-complete)
- 固件版本：`v2.1.1-display-smooth`

## 硬件参数

- OLED：SH1106
- 接口：I2C
- 地址：`0x3C`
- SDA：GPIO8
- SCL：GPIO9
- 库：U8g2
- 模式：软件 I2C

## 下载

- [下载固件 ZIP](https://github.com/nvnmvm/esp32-s3-AIchat-firmware/archive/refs/tags/v2.1.1-display-smooth.zip)
- [下载固件 TAR.GZ](https://github.com/nvnmvm/esp32-s3-AIchat-firmware/archive/refs/tags/v2.1.1-display-smooth.tar.gz)

## 验收

1. 上电后 OLED 正常显示 SH1106 画面，无雪花屏。
2. WiFi 连接时显示 `网络连接中`，连接成功后显示 `WiFi连接成功`，随后显示上海时间。
3. 说“小一小一”后进入录音，OLED 显示 `听取中`。
4. 上传音频和等待云端期间显示 `思考中`。
5. 收到 `answer_text` 后显示 `回答中`。
6. 播放回答音频时只滚动显示回答文字，不再显示“正在播放”。
7. 文字滚动结束并收到 `audio_end` 后显示 `回答完毕`。
8. `回答完毕` 保留约 2 秒后回到等待唤醒。

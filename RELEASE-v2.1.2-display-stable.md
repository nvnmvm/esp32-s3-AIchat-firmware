# v2.1.2-display-stable

阶段二稳定显示闭环版。本版本修复录音结束后旧云端可能发出的残余音频错误导致 OLED 固定显示 `云端错误` 的问题。

## 修复内容

- 固件忽略 `收到音频，但当前没有 start_record。` 这类无害尾包错误。
- 普通云端错误不再永久固定显示，错误页约 2 秒后自动回到 `等待唤醒`。
- 保留 `v2.1.1-display-smooth` 的回答横向滚动和 OLED 显示状态机。

## 配套云端

推荐同步更新云端：[v2.1.3-phase2-stable](https://github.com/nvnmvm/esp32-s3-AIchat/releases/tag/v2.1.3-phase2-stable)

云端 `v2.1.3-phase2-stable` 会从源头忽略录音结束后的残余 PCM 包，不再向 ESP32 发送该错误。

## 下载

- [下载固件 ZIP](https://github.com/nvnmvm/esp32-s3-AIchat-firmware/archive/refs/tags/v2.1.2-display-stable.zip)
- [下载固件 TAR.GZ](https://github.com/nvnmvm/esp32-s3-AIchat-firmware/archive/refs/tags/v2.1.2-display-stable.tar.gz)

## 验收

1. 完成一次阶段二对话后，OLED 不会停留在 `云端错误`。
2. 如果云端仍发送 `收到音频，但当前没有 start_record。`，固件串口输出 `Ignored stray audio error after recording finished.`，OLED 保持当前正常流程。
3. 普通云端错误显示约 2 秒后回到 `等待唤醒`。
4. 回答滚动、`回答完毕`、回到上海时间等待唤醒流程保持正常。

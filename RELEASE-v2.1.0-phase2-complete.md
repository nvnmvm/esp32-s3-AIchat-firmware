# v2.1.0-phase2-complete

阶段二完善版。本版本用于进入阶段三前的稳定过渡，配套云端同名版本 `v2.1.0-phase2-complete`。

## 主要改进

- 统一云端和固件版本号，两个仓库都使用 `v2.1.0-phase2-complete`。
- OLED 支持更稳的阶段二状态显示：`听取中`、`识别结果`、`思考中`、`回答中`、`回答完成`。
- `asr_text` 会显示为识别结果，便于没有喇叭时确认云端解析链路。
- `answer_text` 会保存为最近回答，`audio_start` 不再覆盖回答正文。
- `audio_end` 后保留回答约 8 秒，再回到上海时间等待唤醒界面。
- 长文本按 OLED 宽度拆成两行，超出部分显示省略号，避免完全看不到回答。

## 配套云端

云端 release：[v2.1.0-phase2-complete](https://github.com/nvnmvm/esp32-s3-AIchat/releases/tag/v2.1.0-phase2-complete)

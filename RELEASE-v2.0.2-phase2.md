# v2.0.2-phase2

阶段二固件状态修正版。本版本是 `v2.0.1-phase2` 的后继补丁版本，配套云端 `v2.0.1-phase2`。

## 修复

- 修复云端 `status` 消息覆盖 OLED 状态的问题。
- 云端 `state=recording` 时保持显示 `听取中`。
- 云端 `state=asr` 或 `state=thinking` 时显示 `思考中`。
- 云端 `state=idle` 时回到上海时间等待唤醒界面。
- 保留 `answer_text` 和 `audio_start` 阶段的 `回答中` 显示。

## 配套云端

请使用云端仓库 release：`v2.0.1-phase2`。该版本会把本轮语音解析文本写入临时文件，读取回复后自动删除。

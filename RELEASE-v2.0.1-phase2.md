# v2.0.1-phase2

阶段二固件 OLED 修正版。本版本是 `v2.0.0-phase2` 的后继补丁版本。

## 修复

- OLED 驱动从 Adafruit SSD1306 改为 U8g2 SH1106。
- OLED 初始化改为软件 I2C。
- OLED 地址固定为 `0x3C`。
- OLED 接线固定为 SDA `GPIO8`、SCL `GPIO9`。
- 移除 Adafruit GFX / Adafruit SSD1306 依赖，改用 `olikraus/U8g2`。
- README 和阶段二文档同步改为 SH1106/U8g2 软件 I2C 参数。

## 屏幕状态

- WiFi 连接期间显示 `网络连接中`。
- WiFi 连接成功后显示 `WiFi连接成功`，随后同步并显示上海时间。
- 空闲时显示 `等待唤醒`、上海时间和 `说 小一小一`。
- ASRPRO 发出 `WAKE` 后立即录音并显示 `听取中`。
- 上传云端和等待回答期间显示 `思考中`。
- 云端返回回答文本和播放回答音频期间显示 `回答中`。

## 版本更替

- 阶段一：`v1.0.0-phase1`
- 阶段二初版：`v2.0.0-phase2`
- 阶段二 OLED 修正版：`v2.0.1-phase2`

## 配套云端

云端仍使用 `v2.0.0-phase2`。本次只修复 ESP32-S3 固件 OLED 显示问题。

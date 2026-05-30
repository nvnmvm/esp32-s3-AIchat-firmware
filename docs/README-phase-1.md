# 阶段一 README：WebSocket 通信打通版

阶段一固件只验证 ESP32-S3 与 VPS 云端之间的 WebSocket 双向通信。

## 功能范围

- 连接 WiFi。
- 连接 `ws://VPS_IP:PORT/ws?token=...`。
- 每秒发送 `hello from esp32 ...`。
- 串口打印云端 echo 返回内容。
- 断线后自动重连 WiFi 和 WebSocket。

## 发布版本

- 固件 tag/release：`v1.0.0-phase1`
- 配套云端 tag/release：`v1.0.0-phase1`

## 注意

阶段一不使用 OLED、MS4030 麦克风、ASRPRO 或 MAX98357A。后续阶段 README 不应覆盖本阶段留档。

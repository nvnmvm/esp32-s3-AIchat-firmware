# 阶段二 README：语音和屏幕双输出闭环版

阶段二固件验证 ASRPRO 唤醒、I2S 麦克风上传、SH1106 OLED 显示回答、MAX98357A 播放云端返回 PCM 音频。

## 硬件接线

| 模块 | 模块引脚 | ESP32-S3 引脚 | 说明 |
| --- | --- | --- | --- |
| 0.96 OLED | GND | GND | 共地 |
| 0.96 OLED | VCC | 3V3 | 供电 |
| 0.96 SH1106 OLED | SDA | GPIO8 | U8g2 软件 I2C 数据 |
| 0.96 SH1106 OLED | SCL | GPIO9 | U8g2 软件 I2C 时钟 |
| MS4030 麦克风 | GND | GND | 共地 |
| MS4030 麦克风 | VDD | 3V3 | 供电 |
| MS4030 麦克风 | SD | GPIO6 | I2S 数据输入 |
| MS4030 麦克风 | WS | GPIO5 | I2S LRCLK |
| MS4030 麦克风 | SCK | GPIO4 | I2S BCLK |
| MS4030 麦克风 | L/R | GND | mono |
| ASRPRO | PB5 / UART0 TX | GPIO16 / RX | 输出 WAKE/STOP/CANCEL |
| ASRPRO | PB6 / UART0 RX | GPIO17 / TX | 阶段二可不接 |
| MAX98357A | VIN | 5V 或 3V3 | 推荐 5V/VBUS |
| MAX98357A | GND | GND | 必须共地 |
| MAX98357A | BCLK | GPIO12 | I2S BCLK |
| MAX98357A | LRC | GPIO13 | I2S LRCLK |
| MAX98357A | DIN | GPIO14 | I2S 数据输出 |
| MAX98357A | SD | 3V3 或 GPIO15 | 本固件用 GPIO15 拉高 |
| MAX98357A | SPK+/SPK- | 喇叭 | 只接喇叭，不接 ESP32 GPIO |

如果 ASRPRO PB5 输出是 5V 电平，不要直接接 ESP32-S3 GPIO16，需要降压或电平转换。所有模块必须共地。

OLED 已按实测参数固定为：I2C 地址 `0x3C`、驱动 `SH1106`、库 `U8g2`、模式 `软件 I2C`。如果出现雪花屏，优先确认没有使用 SSD1306 示例或硬件 I2C 初始化。

## ASRPRO 命令

ASRPRO 通过 UART0 9600 波特率发送 ASCII 命令：

| 命令 | 作用 |
| --- | --- |
| `WAKE\n` | 唤醒并立即开始录音 |
| `CANCEL\n` | 取消当前录音或播放 |
| `STOP\n` | 结束当前对话，回到空闲 |

## 固件流程

1. 连接 WiFi，OLED 显示 `网络连接中`。
2. WiFi 连接成功后，OLED 显示 `WiFi连接成功`，然后同步并显示上海时间。
3. 连接云端 WebSocket 后，等待 ASRPRO 发来 `WAKE`。
4. ASRPRO 发出 `WAKE` 后，OLED 显示 `听取中` 并立即开始 I2S 录音。
5. 向云端发送 `{"type":"start_record"}`。
6. 分块上传 16 kHz、16 bit、mono PCM。
7. 上传结束和等待云端回答期间，OLED 显示 `思考中`。
8. 云端返回 `answer_text` 或开始播放音频后，OLED 显示 `回答中`。
9. 云端返回 `audio_start` 和 PCM 音频后，ESP32-S3 通过 I2S 输出到 MAX98357A。
10. 收到 `audio_end` 后回到空闲，并继续显示上海时间。

## 配套云端

云端必须使用配套版本：`v2.0.1-phase2`。

VPS 测试前先清理阶段一旧部署：

```bash
curl -fsSL https://raw.githubusercontent.com/nvnmvm/esp32-s3-AIchat/main/install.sh -o install.sh && sudo bash install.sh --repo https://github.com/nvnmvm/esp32-s3-AIchat.git --clean
```

## 验收标准

- SH1106 OLED 正常显示状态。
- ASRPRO 发出 `WAKE` 后固件进入录音。
- 云端 `state=recording` 不会覆盖 `听取中`，`state=idle` 会回到上海时间。
- 云端日志能看到 PCM 音频上传。
- OLED 显示云端返回的回答文本。
- MAX98357A 能播放云端返回的测试音频。
- `CANCEL` 可以取消当前录音或播放。
- `STOP` 可以回到空闲。

当前 OLED 使用 U8g2 SH1106 软件 I2C，显示中文状态和短文本。

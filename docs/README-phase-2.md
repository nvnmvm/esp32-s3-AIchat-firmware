# 阶段二 README：语音和屏幕双输出闭环版

阶段二固件验证 ASRPRO 唤醒、I2S 麦克风上传、SH1106 OLED 显示回答、MAX98357A 播放云端返回 PCM 音频，目标是完成稳定的 AI 对话核心闭环。阶段三再实现实时流式对话、音频环形缓冲和流式字幕。

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
8. 云端返回 `answer_text` 后，OLED 显示 `回答中`。
9. 短暂停留后，OLED 只显示 AI 回答文字，并从右向左横向滚动。
10. 云端返回 `audio_start` 和 PCM 音频后，ESP32-S3 通过 I2S 输出到 MAX98357A，屏幕继续滚动回答文字，不再显示“正在播放”占用正文区域。
11. 收到 `audio_end` 时只标记音频结束。
12. 回答文字滚动结束且音频播放结束后，OLED 显示 `回答完毕` 约 2 秒，再回到空闲并继续显示上海时间。

OLED 显示由固件内部状态机统一刷新，WebSocket 事件只修改显示状态。这样可以避免 `asr_text`、`answer_text`、`audio_start`、`audio_end` 和云端 `status` 消息短时间连续到达时互相抢屏。

如果旧云端在录音结束后把残余 PCM 包误报为 `收到音频，但当前没有 start_record。`，固件会直接忽略这个无害错误。其他普通云端错误会显示约 2 秒，然后自动回到等待唤醒，不会固定卡在错误页。

回答滚动参数：

```cpp
DISPLAY_FRAME_MS = 40;      // 25 FPS
SCROLL_SPEED_PX_PER_SEC = 35;
ANSWER_INTRO_MS = 700;
ANSWER_DONE_MS = 2000;
```

## 配套云端

推荐使用云端阶段二最新版：[`v2.1.3-phase2-stable`](https://github.com/nvnmvm/esp32-s3-AIchat/releases/tag/v2.1.3-phase2-stable)。

VPS 测试前先清理阶段一旧部署：

```bash
curl -fsSL https://raw.githubusercontent.com/nvnmvm/esp32-s3-AIchat/main/install.sh -o install.sh && sudo bash install.sh --repo https://github.com/nvnmvm/esp32-s3-AIchat.git --clean
```

## 验收标准

- SH1106 OLED 正常显示状态。
- ASRPRO 发出 `WAKE` 后固件进入录音。
- 云端 `state=recording` 不会覆盖 `听取中`，`state=idle` 会回到上海时间。
- 没有喇叭时，OLED 也能显示识别结果和 AI 回答。
- `answer_text` 后先显示 `回答中`，随后只显示 AI 回答文字。
- `audio_start` 不覆盖回答正文，不显示“正在播放”占用文字区域。
- AI 回答文字从右向左横向滚动。
- `audio_end` 只标记音频结束，必须等文字滚动也结束后才显示 `回答完毕`。
- `回答完毕` 保留约 2 秒后回到上海时间等待唤醒。
- 旧云端误发的“没有 start_record”尾包错误不会卡住 OLED。
- 普通 `error` 页面显示约 2 秒后会自动回到等待唤醒。
- 云端日志能看到 PCM 音频上传。
- OLED 显示云端返回的回答文本。
- MAX98357A 能播放云端返回的测试音频。
- `CANCEL` 可以取消当前录音或播放。
- `STOP` 可以回到空闲。

当前 OLED 使用 U8g2 SH1106 软件 I2C，显示中文状态和回答滚动文本。

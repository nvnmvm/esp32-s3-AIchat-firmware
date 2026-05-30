# ESP32-S3 AI 对话机器人固件

当前版本：`v2.1.1-display-smooth`，阶段二显示闭环完善版。

本仓库是 ESP32-S3 固件。阶段二使用 ASRPRO 唤醒，采集 MS4030 I2S 麦克风音频上传到云端，再接收云端返回的回答文本和 PCM 音频，实现 SH1106 OLED 显示与 MAX98357A 播放。

## 配套仓库

- 云端仓库：https://github.com/nvnmvm/esp32-s3-AIchat.git
- 固件仓库：https://github.com/nvnmvm/esp32-s3-AIchat-firmware.git

## 阶段 README

- 阶段一：`docs/README-phase-1.md`
- 阶段二：`docs/README-phase-2.md`

每个阶段都通过 tag 和 GitHub Release 固定版本，后续阶段不覆盖前一阶段说明。

## 硬件接线

| 模块 | 模块引脚 | ESP32-S3 引脚 |
| --- | --- | --- |
| SH1106 OLED SDA | SDA | GPIO8 |
| SH1106 OLED SCL | SCL | GPIO9 |
| MS4030 SCK | SCK | GPIO4 |
| MS4030 WS | WS | GPIO5 |
| MS4030 SD | SD | GPIO6 |
| ASRPRO TX | PB5 / UART0 TX | GPIO16 |
| ASRPRO RX | PB6 / UART0 RX | GPIO17，可选 |
| MAX98357A BCLK | BCLK / BCK | GPIO12 |
| MAX98357A LRC | LRC / LRCLK / WS | GPIO13 |
| MAX98357A DIN | DIN | GPIO14 |
| MAX98357A SD | SD | GPIO15 或直接 3V3 |

所有模块必须共地。MAX98357A 推荐使用 5V/VBUS 供电，喇叭只接 `SPK+` / `SPK-`。

OLED 使用 U8g2 驱动，配置为 `SH1106`、地址 `0x3C`、软件 I2C，SDA 为 GPIO8，SCL 为 GPIO9。不要按 SSD1306 或硬件 I2C 测试代码来判断本固件。

## 云端先部署

阶段二 VPS 测试前先删除阶段一旧部署，再部署阶段二云端：

```bash
curl -fsSL https://raw.githubusercontent.com/nvnmvm/esp32-s3-AIchat/main/install.sh -o install.sh && sudo bash install.sh --repo https://github.com/nvnmvm/esp32-s3-AIchat.git --clean
```

部署完成后记录：

- VPS IP 或域名
- WebSocket 端口，默认 `8000`
- WebSocket token

## 配置固件

复制配置模板：

Windows PowerShell：

```powershell
Copy-Item include/config.example.h include/config.h
```

Linux / macOS：

```bash
cp include/config.example.h include/config.h
```

编辑 `include/config.h`：

```c
#define WIFI_SSID "你的WiFi名称"
#define WIFI_PASSWORD "你的WiFi密码"
#define WS_HOST "你的VPS公网IP或域名"
#define WS_PORT 8000
#define WS_TOKEN "云端部署时生成的token"
```

`include/config.h` 已加入 `.gitignore`，不会提交到 GitHub。

## 烧录

Windows PowerShell：

```powershell
.\scripts\flash.ps1
```

手动命令：

```bash
pio run -t upload
pio device monitor
```

## ASRPRO 命令

ASRPRO 通过 UART0 9600 波特率向 ESP32-S3 发送：

```text
WAKE
CANCEL
STOP
```

`WAKE` 开始一轮录音；`CANCEL` 取消录音或播放；`STOP` 回到空闲。

## OLED 状态

阶段二 OLED 已切换为 U8g2 SH1106 软件 I2C，并使用中文字体显示关键状态：

| 场景 | OLED 显示 |
| --- | --- |
| 正在连接 WiFi | `网络连接中` |
| WiFi 已连接 | `WiFi连接成功` |
| 空闲等待唤醒 | `等待唤醒`、`上海时间 HH:MM:SS`、`说 小一小一` |
| ASRPRO 发出 `WAKE` | `听取中` |
| 已上传音频并等待云端返回 | `思考中` |
| 云端返回 `answer_text` | `回答中`，短暂停留后进入回答正文 |
| 云端返回 `audio_start` 并播放音频 | 只显示 AI 回答文字，回答从右向左横向滚动 |
| 回答文字滚动结束且收到 `audio_end` | `回答完毕`，保留约 2 秒后回到等待唤醒 |

固件会按云端 `status.state` 精确切换显示状态：`recording` 保持 `听取中`，`asr` / `thinking` 显示 `思考中`，`idle` 在非回答阶段才回到上海时间。OLED 刷新统一由固件显示状态机处理，WebSocket 事件只修改显示状态，避免识别结果、回答中、播放中、回答完成等页面互相抢屏。

没有喇叭时也可以通过 OLED 验证闭环：固件会显示 `asr_text` 识别结果，收到 `answer_text` 后显示 `回答中`，随后滚动显示 AI 回答；`audio_start` 不再占用回答正文区域显示“正在播放”，`audio_end` 只标记音频已结束，必须等文字滚动也结束后才显示 `回答完毕`。

## 正常串口输出

```text
ESP32-S3 AI voice phase 2 voice/screen loopback
WiFi connected, IP=...
WebSocket connected: ...
ASRPRO command: WAKE
Sent PCM chunk: 1280 bytes
Cloud JSON type=answer_text text=...
Played audio chunk: ...
```

## 注意

当前 OLED 使用 U8g2 SH1106 软件 I2C 显示中文状态和回答文本，回答滚动刷新间隔为 40ms，约 25 FPS。阶段二重点是把唤醒、录音、云端 AI 回答、OLED 文本、MAX98357A 播放闭环跑通；阶段三再做音频环形缓冲、真正边收边播、ASR partial 字幕和 DeepSeek 流式文字同步。

## 配套云端

推荐使用云端仓库阶段二最新版：[v2.1.2-phase2-complete](https://github.com/nvnmvm/esp32-s3-AIchat/releases/tag/v2.1.2-phase2-complete)。本固件仍使用阶段二 JSON + PCM WebSocket 协议，不需要云端做阶段三流式改造。

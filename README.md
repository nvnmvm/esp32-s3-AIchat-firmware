# ESP32-S3 AI 对话机器人固件

当前版本：`v2.0.2-phase2`，阶段二语音和屏幕双输出闭环版状态修正版。

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
| 云端返回回答文本或音频播放中 | `回答中` |

固件会按云端 `status.state` 精确切换屏幕状态：`recording` 保持 `听取中`，`asr` / `thinking` 显示 `思考中`，`idle` 回到上海时间，避免云端状态消息覆盖录音界面。

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

当前 OLED 使用 U8g2 SH1106 软件 I2C 显示中文状态和短文本；阶段二重点是把语音上传、文本显示、音频播放闭环跑通。

# v2.0.0-phase2

阶段二语音和屏幕双输出闭环版。

## 新增

- ASRPRO UART 输入：GPIO16 接 PB5 / UART0 TX，读取 `WAKE`、`STOP`、`CANCEL`。
- MS4030 I2S 麦克风采集：GPIO4/5/6，16 kHz / 16 bit / mono PCM。
- OLED 显示：GPIO8/9，显示连接状态、录音状态、云端回答文本。
- MAX98357A 播放：GPIO12/13/14，GPIO15 控制 SD 拉高。
- 阶段二 JSON + PCM WebSocket 协议。
- 新增 `include/pins.h` 集中管理硬件引脚。
- 新增阶段一和阶段二独立 README 留档。

## 配套云端

请先发布并部署云端同名版本：`v2.0.0-phase2`。

VPS 测试阶段二前建议使用云端安装脚本的 `--clean` 参数删除阶段一旧部署。

## 注意

当前 OLED 使用默认 ASCII 字体，主要用于状态和短文本显示。正式中文字体、正式 ASR/LLM/TTS 体验会在后续阶段增强。

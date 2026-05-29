# ESP32-S3 AI 对话机器人固件

这是 ESP32-S3 AI 对话机器人“阶段一通信打通版”的固件项目。

本阶段只验证 ESP32-S3 和云端 VPS 之间的 WebSocket 双向通信，不接入麦克风、喇叭、ASR、AI 大模型或 TTS。

## 当前阶段

阶段一目标：

- ESP32-S3 连接本地 WiFi。
- ESP32-S3 连接云端 WebSocket 服务。
- ESP32-S3 每秒发送一条 `hello from esp32 ...` 测试消息。
- 云端收到消息后原样返回。
- ESP32-S3 串口打印云端返回内容。
- 断线后自动重连 WiFi 和 WebSocket。

## 配套云端项目

云端服务仓库：

```bash
https://github.com/nvnmvm/esp32-s3-AIchat.git
```

云端部署成功后，会得到：

- VPS 公网 IP 或域名
- WebSocket 端口，默认 `8000`
- WebSocket 令牌 `WS_TOKEN`

这些值需要填入本固件项目的本地配置文件 `include/config.h`。

## 克隆固件项目

```bash
git clone https://github.com/nvnmvm/esp32-s3-AIchat-firmware.git
cd esp32-s3-AIchat-firmware
```

## 需要安装的软件

烧录前需要安装：

- VS Code
- PlatformIO 插件
- ESP32-S3 开发板 USB 驱动

如果你已经能运行 `pio` 命令，也可以不用 VS Code，直接用命令行烧录。

## 修改配置

先复制配置模板：

```bash
cp include/config.example.h include/config.h
```

Windows PowerShell：

```powershell
Copy-Item include/config.example.h include/config.h
```

然后打开：

```text
include/config.h
```

修改下面这些值：

```c
#define WIFI_SSID "你的WiFi名称"
#define WIFI_PASSWORD "你的WiFi密码"

#define WS_HOST "你的VPS公网IP或域名"
#define WS_PORT 8000
#define WS_TOKEN "云端部署时生成或自定义的WebSocket令牌"
```

说明：

- `WIFI_SSID`：本地 WiFi 名称。
- `WIFI_PASSWORD`：本地 WiFi 密码。
- `WS_HOST`：云端 VPS 公网 IP 或域名，不要带 `ws://`。
- `WS_PORT`：云端 WebSocket 端口，默认 `8000`，如果云端部署时改了端口，这里也要同步修改。
- `WS_TOKEN`：云端部署时生成或自定义的 WebSocket 令牌，必须和云端 `.env` 里的 `WS_TOKEN` 一致。

注意：`include/config.h` 已经加入 `.gitignore`，只在你本地使用，不提交到 GitHub。仓库里只保留 `include/config.example.h` 模板。

AI API Key 只应该放在云端 `.env` 里，不应该写进 ESP32 固件。

## 一键烧录

Windows PowerShell：

```powershell
.\scripts\flash.ps1
```

这个脚本会执行：

```bash
pio run -t upload
pio device monitor
```

## 手动烧录

```bash
pio run -t upload
pio device monitor
```

## 正常串口输出

烧录成功并连接云端后，串口应看到类似输出：

```text
ESP32-S3 AI voice phase 1 WebSocket echo test
WiFi connected, IP=...
WebSocket connected: ...
Sent: hello from esp32 ...
WebSocket text received (... bytes): hello from esp32 ...
```

如果看到 `Send skipped: WebSocket is not connected`，说明 WebSocket 还没有连上，需要检查 VPS IP、端口、防火墙和 `WS_TOKEN`。

如果串口提示 `Config error`，说明 `include/config.h` 里还有占位值没有改。先修改 WiFi、VPS 地址和 `WS_TOKEN`，再重新烧录。

## 工程化说明

本固件阶段一保持功能简单，但已经加入基础自检：

- 开机检查 `WIFI_SSID`、`WIFI_PASSWORD`、`WS_HOST`、`WS_TOKEN` 是否仍是占位值。
- `include/config.h` 作为本地私有配置，不提交到 GitHub。
- 配置错误时停止继续联网，并在串口打印明确提示。
- WebSocket 自动重连和心跳保持开启，方便长时间测试。
- `.github/workflows/ci.yml`：后续推送到 GitHub 后自动执行 PlatformIO 编译检查。

更完整的固件成熟度路线图见：

```text
docs/maturity-roadmap.md
```

## 常见问题

### WiFi 连不上

检查：

- `WIFI_SSID` 是否正确。
- `WIFI_PASSWORD` 是否正确。
- ESP32-S3 是否处在 WiFi 覆盖范围内。
- WiFi 是否为 2.4GHz。多数 ESP32 开发板不支持 5GHz WiFi。

### WebSocket 连不上

检查：

- 云端服务是否已经部署成功。
- VPS 是否放行 TCP `8000` 端口。
- 云平台安全组是否放行 TCP `8000` 端口。
- `WS_HOST` 是否只填写 IP 或域名，不要写 `ws://`。
- `WS_TOKEN` 是否和云端 `.env` 完全一致。

### 烧录失败

检查：

- 开发板是否为 ESP32-S3。
- USB 数据线是否支持数据传输。
- PlatformIO 是否安装完成。
- 串口是否被其他软件占用。

## 后续阶段迭代计划

本仓库会和云端仓库一起迭代。

计划按阶段推进：

- `phase-1`：WebSocket 通信打通，只做 echo 测试。
- `phase-2`：音频采集上传，云端保存和识别测试。
- `phase-3`：ASR、LLM、TTS 和 ESP32 播放闭环。
- `phase-4`：流式优化、稳定性和工程化增强。

## GitHub 版本管理方式

后续每完成一个阶段，会按下面方式管理版本：

- `main` 分支：始终保存当前最新可用版本。
- `phase-1`、`phase-2`、`phase-3`、`phase-4` 分支：用于对应阶段开发和修复。
- `v1.0.0-phase1`、`v2.0.0-phase2`、`v3.0.0-phase3`、`v4.0.0-phase4` 标签：用于固定每个阶段完成时的代码。
- GitHub Release：每个标签会发布一个 Release，说明本阶段功能、烧录方式、云端配套版本和注意事项。
- README：每个阶段完成时都会同步更新 README；Git tag 会保存当时的 README，所以以后可以回看每个阶段对应的说明。

推荐工作流：

```bash
git checkout -b phase-2
# 开发阶段二
git commit -m "Add phase 2 audio upload firmware"
git push -u origin phase-2

# 阶段二稳定后合并到 main，并打标签
git checkout main
git merge phase-2
git tag v2.0.0-phase2
git push origin main --tags
```

实际后续我会直接基于这两个仓库继续改，不会重新另起项目。

# 固件工程成熟度路线图

阶段一固件保持最小可用，只验证 WiFi 和 WebSocket 通信。

## 当前已经具备

- 所有用户配置集中在本地 `include/config.h`，仓库只提交 `include/config.example.h`。
- `scripts/init-config.ps1` 和 `scripts/init-config.sh` 会自动从模板生成本地配置文件，避免新手误改模板。
- 烧录脚本会在缺少 `include/config.h` 时自动生成文件并停止烧录，提醒用户先填写配置。
- 烧录脚本会检查常见占位值，配置未填好时不会继续执行上传。
- 开机检查 WiFi、VPS 地址和 WebSocket 令牌是否仍是占位值。
- WebSocket 自动重连和心跳。
- 串口输出连接状态和 echo 测试结果。
- GitHub Actions 后续会执行 PlatformIO 编译检查。
- AI API Key 只放云端，不写入 ESP32 固件。

## 后续阶段再做

- 阶段二：增加 I2S 麦克风采集、音频分片、上传节流和二进制帧协议。
- 阶段三：增加 TTS 音频接收、喇叭播放、对话状态和错误恢复策略。
- 阶段四：增加流式优化、设备唯一 ID 生成、OTA 升级和更完整的硬件适配说明。

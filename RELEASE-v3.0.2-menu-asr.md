# v3.0.2-menu-asr

## 重点

- 配套云端 `v3.0.2-menu-asr`。
- `CLOUD_PROTOCOL_VERSION` 更新为 `302`。
- `start_record` metadata 上报固件版本 `v3.0.2-menu-asr`。
- 保持 3.0.1 的 OLED 流转：不显示 ASR 识别文本页，不显示回答总览页，直接滚动显示回答。
- 保持 3.0.1 的断连兼容：回答结束附近的正常重连不强制显示“云端断开”。

## 升级

```bash
cp include/config.example.h include/config.h
```

编辑 Wi-Fi、VPS 地址、端口和 token 后构建：

```bash
pio run
```

烧录：

```bash
pio run -t upload
```

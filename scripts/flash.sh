#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."

if [ ! -f "include/config.h" ]; then
  cp "include/config.example.h" "include/config.h"
  echo "首次运行已自动生成 include/config.h。"
  echo "请先填写 WiFi、服务器地址和 WS_TOKEN，然后重新运行烧录脚本。"
  echo "不要修改 include/config.example.h。"
  exit 1
fi

for placeholder in \
  "YOUR_WIFI_SSID" \
  "YOUR_WIFI_PASSWORD" \
  "YOUR_VPS_IP_OR_DOMAIN" \
  "CHANGE_ME_TO_THE_CLOUD_TOKEN"; do
  if grep -q "$placeholder" "include/config.h"; then
    echo "include/config.h 里仍然有占位值：$placeholder"
    echo "请先修改 include/config.h，填写 WiFi、服务器地址和 WS_TOKEN，然后重新运行烧录脚本。"
    echo "不要修改 include/config.example.h。"
    exit 1
  fi
done

pio run -t upload
pio device monitor

#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."

config_example="include/config.example.h"
config_file="include/config.h"

if [ -f "$config_file" ]; then
  echo "include/config.h 已存在，不会覆盖。"
  exit 0
fi

if [ ! -f "$config_example" ]; then
  echo "找不到 include/config.example.h，无法生成配置文件。" >&2
  exit 1
fi

cp "$config_example" "$config_file"

echo "已生成 include/config.h。"
echo "请打开 include/config.h，填写 WIFI_SSID、WIFI_PASSWORD、WS_HOST、WS_PORT、WS_TOKEN。"
echo "不要修改 include/config.example.h。"

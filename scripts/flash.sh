#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."

if [ ! -f "include/config.h" ]; then
  cp "include/config.example.h" "include/config.h"
  echo "Created include/config.h from include/config.example.h"
  echo "Edit include/config.h first, then run this script again."
  exit 1
fi

pio run -t upload
pio device monitor

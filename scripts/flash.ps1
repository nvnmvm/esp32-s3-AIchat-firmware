Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Msg([string]$Base64) {
    [System.Text.Encoding]::UTF8.GetString([System.Convert]::FromBase64String($Base64))
}

$ProjectRoot = Split-Path -Parent $PSScriptRoot
Set-Location $ProjectRoot

if (-not (Test-Path "include/config.h")) {
    Copy-Item "include/config.example.h" "include/config.h"
    Write-Host (Msg "6aaW5qyh6L+Q6KGM5bey6Ieq5Yqo55Sf5oiQIGluY2x1ZGUvY29uZmlnLmjjgII=")
    Write-Host (Msg "6K+35YWI5aGr5YaZIFdpRmnjgIHmnI3liqHlmajlnLDlnYDlkowgV1NfVE9LRU7vvIznhLblkI7ph43mlrDov5DooYzng6flvZXohJrmnKzjgII=")
    Write-Host (Msg "5LiN6KaB5L+u5pS5IGluY2x1ZGUvY29uZmlnLmV4YW1wbGUuaOOAgg==")
    exit 1
}

$ConfigText = Get-Content "include/config.h" -Raw
$Placeholders = @(
    "YOUR_WIFI_SSID",
    "YOUR_WIFI_PASSWORD",
    "YOUR_VPS_IP_OR_DOMAIN",
    "CHANGE_ME_TO_THE_CLOUD_TOKEN"
)

foreach ($Placeholder in $Placeholders) {
    if ($ConfigText.Contains($Placeholder)) {
        Write-Host ((Msg "aW5jbHVkZS9jb25maWcuaCDph4zku43nhLbmnInljaDkvY3lgLzvvJo=") + $Placeholder)
        Write-Host (Msg "6K+35YWI5aGr5YaZIFdpRmnjgIHmnI3liqHlmajlnLDlnYDlkowgV1NfVE9LRU7vvIznhLblkI7ph43mlrDov5DooYzng6flvZXohJrmnKzjgII=")
        Write-Host (Msg "5LiN6KaB5L+u5pS5IGluY2x1ZGUvY29uZmlnLmV4YW1wbGUuaOOAgg==")
        exit 1
    }
}

pio run -t upload
pio device monitor

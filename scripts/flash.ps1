Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$ProjectRoot = Split-Path -Parent $PSScriptRoot
Set-Location $ProjectRoot

if (-not (Test-Path "include/config.h")) {
    Copy-Item "include/config.example.h" "include/config.h"
    Write-Host "Created include/config.h from include/config.example.h"
    Write-Host "Edit include/config.h first, then run this script again."
    exit 1
}

pio run -t upload
pio device monitor

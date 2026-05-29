Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Msg([string]$Base64) {
    [System.Text.Encoding]::UTF8.GetString([System.Convert]::FromBase64String($Base64))
}

$ProjectRoot = Split-Path -Parent $PSScriptRoot
$ConfigExample = Join-Path $ProjectRoot "include/config.example.h"
$ConfigFile = Join-Path $ProjectRoot "include/config.h"

Set-Location $ProjectRoot

if (Test-Path $ConfigFile) {
    Write-Host (Msg "aW5jbHVkZS9jb25maWcuaCDlt7LlrZjlnKjvvIzkuI3kvJropobnm5bjgII=")
    exit 0
}

if (-not (Test-Path $ConfigExample)) {
    Write-Error (Msg "5om+5LiN5YiwIGluY2x1ZGUvY29uZmlnLmV4YW1wbGUuaO+8jOaXoOazleeUn+aIkOmFjee9ruaWh+S7tuOAgg==")
    exit 1
}

Copy-Item $ConfigExample $ConfigFile

Write-Host (Msg "5bey55Sf5oiQIGluY2x1ZGUvY29uZmlnLmjjgII=")
Write-Host (Msg "6K+35omT5byAIGluY2x1ZGUvY29uZmlnLmjvvIzloavlhpkgV0lGSV9TU0lE44CBV0lGSV9QQVNTV09SROOAgVdTX0hPU1TjgIFXU19QT1JU44CBV1NfVE9LRU7jgII=")
Write-Host (Msg "5LiN6KaB5L+u5pS5IGluY2x1ZGUvY29uZmlnLmV4YW1wbGUuaOOAgg==")

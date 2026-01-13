param(
  [Parameter(Mandatory=$true)]
  [string]$HexPath
)

$ErrorActionPreference = "Stop"

if (!(Test-Path $HexPath)) {
  throw "Hex file not found: $HexPath"
}

$dl = (Get-Volume | Where-Object { $_.FileSystemLabel -eq "MICROBIT" }).DriveLetter
if (-not $dl) {
  throw "MICROBIT drive not found. Plug in micro:bit and ensure it appears as MICROBIT."
}

$destName = Split-Path $HexPath -Leaf
Copy-Item $HexPath "$($dl):\$destName" -Force
Write-Host "Copied $HexPath -> $($dl):\$destName"

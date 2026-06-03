param(
    [string]$ConfigPath = "$env:USERPROFILE\.codex\config.toml",
    [string]$PythonExe = "$env:LOCALAPPDATA\CodexMCP\unreal-engine-mcp\Scripts\python.exe",
    [string]$ServerScript = "F:\Programming\unreal-engine-mcp\Python\unreal_mcp_server_advanced.py"
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path -LiteralPath $ConfigPath)) {
    throw "Codex config not found: $ConfigPath"
}

if (-not (Test-Path -LiteralPath $PythonExe)) {
    throw "MCP Python executable not found: $PythonExe"
}

if (-not (Test-Path -LiteralPath $ServerScript)) {
    throw "Unreal MCP server script not found: $ServerScript"
}

$config = Get-Content -LiteralPath $ConfigPath -Raw

if ($config -match '(?m)^\[mcp_servers\.unreal_mcp\]\s*$') {
    Write-Host "Codex Unreal MCP server is already configured."
    exit 0
}

$block = @"

[mcp_servers.unreal_mcp]
args = ['$ServerScript']
command = '$PythonExe'
startup_timeout_sec = 120
"@

Add-Content -LiteralPath $ConfigPath -Value $block
Write-Host "Added Codex Unreal MCP server configuration to $ConfigPath"

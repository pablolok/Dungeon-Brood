$ErrorActionPreference = "Stop"

$workspace = Split-Path -Parent $MyInvocation.MyCommand.Path
$project = Join-Path $workspace "DungeonBrood.uproject"
$logPath = Join-Path $workspace "Saved\Logs\DungeonBrood.log"
$screenshotPath = Join-Path $workspace "Saved\Automation\WindowedLevel5EnvironmentViewport.png"
Remove-Item -Path $logPath -Force -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Path (Split-Path -Parent $screenshotPath) -Force | Out-Null
Remove-Item -Path $screenshotPath -Force -ErrorAction SilentlyContinue

function Find-UnrealRoot {
    $candidates = @(
        $env:UE_ROOT,
        $env:UE_5_7_ROOT,
        "C:\Program Files\Epic Games\UE_5.7.4",
        "C:\Program Files\Epic Games\UE_5.7",
        "D:\Epic Games\UE_5.7.4",
        "D:\Epic Games\UE_5.7",
        "E:\EpicGames\UE_5.7.4",
        "E:\EpicGames\UE_5.7"
    ) | Where-Object { $_ }

    foreach ($candidate in $candidates) {
        if (Test-Path (Join-Path $candidate "Engine\Binaries\Win64\UnrealEditor.exe")) {
            return $candidate
        }
    }

    throw "Unreal Engine 5.7 was not found. Set UE_ROOT or UE_5_7_ROOT."
}

function Wait-LogPattern {
    param(
        [string] $Pattern,
        [int] $TimeoutSeconds = 70
    )

    $deadline = (Get-Date).AddSeconds($TimeoutSeconds)
    while ((Get-Date) -lt $deadline) {
        if (Test-Path $logPath) {
            $content = Get-Content -Path $logPath -Raw
            if ($content -match $Pattern) {
                return
            }
        }

        Start-Sleep -Milliseconds 500
    }

    throw "Timed out waiting for log pattern: $Pattern"
}

$unrealRoot = Find-UnrealRoot
$unrealEditor = Join-Path $unrealRoot "Engine\Binaries\Win64\UnrealEditor.exe"
$unreal = $null

try {
    $arguments = @(
        $project,
        "-game",
        "-log",
        "-windowed",
        "-ResX=1280",
        "-ResY=720",
        "-WinX=140",
        "-WinY=140",
        "-ExecCmds=BroodDungeonLevel5EnvironmentVisualSmokeTest"
    )
    $unreal = Start-Process -FilePath $unrealEditor -ArgumentList $arguments -WorkingDirectory $workspace -PassThru

    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_EDITOR_AUTHORED_MAP_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_ENVIRONMENT_FOREGROUND_OCCLUDERS_HIDDEN_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_ENVIRONMENT_LEFT_EDGE_CLEANUP_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_ENVIRONMENT_PROXY_CLUTTER_HIDDEN_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_ENVIRONMENT_POE_CAMERA_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_ENVIRONMENT_SCREENSHOT_REQUESTED"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_ENVIRONMENT_VISUAL_SMOKE_PASSED"

    $deadline = (Get-Date).AddSeconds(20)
    while ((Get-Date) -lt $deadline -and (-not (Test-Path $screenshotPath))) {
        Start-Sleep -Milliseconds 500
    }

    if (-not (Test-Path $screenshotPath) -or (Get-Item $screenshotPath).Length -le 0) {
        throw "The Level 5 environment screenshot was not written: $screenshotPath"
    }

    Write-Host "Dungeon Level 5 environment visual smoke passed: screenshot saved to $screenshotPath"
}
finally {
    if ($unreal -and -not $unreal.HasExited) {
        Wait-Process -Id $unreal.Id -Timeout 12 -ErrorAction SilentlyContinue
        if (-not $unreal.HasExited) {
            Stop-Process -Id $unreal.Id -Force -ErrorAction SilentlyContinue
        }
    }
}

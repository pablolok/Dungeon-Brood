$ErrorActionPreference = "Stop"

$workspace = Split-Path -Parent $MyInvocation.MyCommand.Path
$project = Join-Path $workspace "DungeonBrood.uproject"
$logPath = Join-Path $workspace "Saved\Logs\DungeonBrood.log"
$activeLogPath = $null
$scriptStart = Get-Date
$screenshotPath = Join-Path $workspace "Saved\Automation\WindowedJailerHuskViewport.png"
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
        "D:\Program Files\Epic Games\UE_5.7.4",
        "D:\Program Files\Epic Games\UE_5.7",
        "E:\EpicGames\UE_5.7.4",
        "E:\EpicGames\UE_5.7",
        "E:\Epic Games\UE_5.7.4",
        "E:\Epic Games\UE_5.7",
        "E:\Program Files\Epic Games\UE_5.7.4",
        "E:\Program Files\Epic Games\UE_5.7"
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
        $candidateLogs = if ($script:activeLogPath -and (Test-Path $script:activeLogPath)) {
            @(Get-Item $script:activeLogPath)
        } else {
            Get-ChildItem -Path (Join-Path $workspace "Saved\Logs") -Filter "DungeonBrood*.log" -ErrorAction SilentlyContinue |
                Where-Object { $_.LastWriteTime -ge $script:scriptStart.AddSeconds(-2) } |
                Sort-Object LastWriteTime -Descending
        }

        foreach ($candidateLog in $candidateLogs) {
            $content = Get-Content -Path $candidateLog.FullName -Raw
            if ($content -match $Pattern) {
                if (-not $script:activeLogPath) {
                    $script:activeLogPath = $candidateLog.FullName
                }
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
        "-WinX=120",
        "-WinY=120",
        "-ExecCmds=BroodDungeonLevel5JailerVisualSmokeTest"
    )
    $unreal = Start-Process -FilePath $unrealEditor -ArgumentList $arguments -WorkingDirectory $workspace -PassThru

    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_JAILER_HUSK_VISUAL_SMOKE_STARTED"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_MOOD_LIGHTING_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_MODULAR_FLOOR_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_LOW_PROFILE_ROUTE_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_ROUTE_LAYOUT_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_ROUTE_COLLISION_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_JAILER_BOSS_ROOM_AUTHORED_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_JAILER_HUSK_DRESSING_READY|BROOD_DUNGEON_LEVEL5_JAILER_HUSK_FALLBACK_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_JAILER_HUSK_BESPOKE_SILHOUETTE_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_JAILER_HUSK_CHAIN_MOTION_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_CHAMBER_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_PACK_ASSETS_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_CEILING_CLOSED_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_PRISON_ROOM_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_IN_LEVEL_ROOM_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_FOREGROUND_BARS_HIDDEN_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_TEXT_AND_BACKDROP_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_STABILIZED_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_GROUNDED_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_DISTRACTIONS_HIDDEN_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_JAILER_HUSK_PLACEHOLDER_BODY_HIDDEN_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_JAILER_HUSK_VERTICAL_BODY_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_JAILER_HUSK_SCREENSHOT_REQUESTED"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_JAILER_HUSK_VISUAL_SMOKE_PASSED"

    $deadline = (Get-Date).AddSeconds(20)
    while ((Get-Date) -lt $deadline -and (-not (Test-Path $screenshotPath))) {
        Start-Sleep -Milliseconds 500
    }

    if (-not (Test-Path $screenshotPath) -or (Get-Item $screenshotPath).Length -le 0) {
        throw "The Jailer Husk screenshot was not written: $screenshotPath"
    }

    Write-Host "Dungeon Level 5 Jailer Husk visual smoke passed: screenshot saved to $screenshotPath"
}
finally {
    if ($unreal -and -not $unreal.HasExited) {
        Wait-Process -Id $unreal.Id -Timeout 12 -ErrorAction SilentlyContinue
        if (-not $unreal.HasExited) {
            Stop-Process -Id $unreal.Id -Force -ErrorAction SilentlyContinue
        }
    }
}

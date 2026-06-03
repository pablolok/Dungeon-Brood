$ErrorActionPreference = "Stop"

$workspace = Split-Path -Parent $MyInvocation.MyCommand.Path
$project = Join-Path $workspace "DungeonBrood.uproject"
$logPath = Join-Path $workspace "Saved\Logs\DungeonBrood.log"
$screenshotPath = Join-Path $workspace "Saved\Automation\WindowedLevel5CompleteViewport.png"
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
        "-WinX=80",
        "-WinY=80",
        "-ExecCmds=BroodDungeonLevel5CompletionVisualSmokeTest"
    )
    $unreal = Start-Process -FilePath $unrealEditor -ArgumentList $arguments -WorkingDirectory $workspace -PassThru

    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_COMPLETION_VISUAL_SMOKE_STARTED"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_MOOD_LIGHTING_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_MODULAR_FLOOR_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_LOW_PROFILE_ROUTE_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_ROUTE_LAYOUT_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_ROUTE_COLLISION_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_JAILER_APPROACH_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_JAILER_BOSS_ROOM_AUTHORED_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_EXIT_ARCHITECTURE_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_EXIT_DESCENT_ROOM_AUTHORED_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_BROKEN_THRESHOLD_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_EXIT_COMPOSITION_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_JAILER_HUSK_DRESSING_READY|BROOD_DUNGEON_LEVEL5_JAILER_HUSK_FALLBACK_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_EXIT_UNLOCKED"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_EXIT_UNLOCKED_ASSET_FEEDBACK_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_COMPLETE"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_COMPLETION_ANGLED_OCCLUDERS_HIDDEN_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_COMPLETION_REVIEW_CAMERA_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_COMPLETION_SCREENSHOT_REQUESTED"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_COMPLETION_VISUAL_SMOKE_PASSED"

    $deadline = (Get-Date).AddSeconds(20)
    while ((Get-Date) -lt $deadline -and (-not (Test-Path $screenshotPath))) {
        Start-Sleep -Milliseconds 500
    }

    if (-not (Test-Path $screenshotPath) -or (Get-Item $screenshotPath).Length -le 0) {
        throw "The Dungeon Level 5 completion screenshot was not written: $screenshotPath"
    }

    Write-Host "Dungeon Level 5 visual completion smoke passed: screenshot saved to $screenshotPath"
}
finally {
    if ($unreal -and -not $unreal.HasExited) {
        Wait-Process -Id $unreal.Id -Timeout 12 -ErrorAction SilentlyContinue
        if (-not $unreal.HasExited) {
            Stop-Process -Id $unreal.Id -Force -ErrorAction SilentlyContinue
        }
    }
}

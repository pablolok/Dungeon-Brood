$ErrorActionPreference = "Stop"

$workspace = Split-Path -Parent $MyInvocation.MyCommand.Path
$project = Join-Path $workspace "DungeonBrood.uproject"
$logPath = Join-Path $workspace "Saved\Logs\DungeonBrood.log"
$activeLogPath = $null
$scriptStart = Get-Date
$screenshotPath = Join-Path $workspace "Saved\Automation\WindowedSurfaceTrialViewport.png"
Remove-Item -Path $logPath -Force -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Path (Split-Path -Parent $screenshotPath) -Force | Out-Null
Remove-Item -Path $screenshotPath -Force -ErrorAction SilentlyContinue

function Find-UnrealRoot {
    $candidates = @(
        $env:UE_ROOT,
        $env:UE_5_7_ROOT,
        "C:\Program Files\Epic Games\UE_5.7.4",
        "C:\Program Files\Epic Games\UE_5.7",
        "E:\EpicGames\UE_5.7",
        "E:\Epic Games\UE_5.7"
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
        "-WinX=140",
        "-WinY=140",
        "-ExecCmds=BroodDungeonLevel5SurfaceTrialVisualSmokeTest"
    )
    $unreal = Start-Process -FilePath $unrealEditor -ArgumentList $arguments -WorkingDirectory $workspace -PassThru

    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_VISUAL_SMOKE_STARTED"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_SURFACE_TEMPTATION_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_ENTERED"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_GUARDS_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_HUMANOID_SILHOUETTES_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_DEATH_READY"
    Wait-LogPattern "THIS_BODY_WAS_NOT_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_REVIEW_OCCLUDERS_HIDDEN_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_REVIEW_ASSET_STAGE_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_REVIEW_STAGED_SILHOUETTES_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_SCREENSHOT_REQUESTED"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_VISUAL_SMOKE_PASSED"

    $deadline = (Get-Date).AddSeconds(20)
    while ((Get-Date) -lt $deadline -and (-not (Test-Path $screenshotPath))) {
        Start-Sleep -Milliseconds 500
    }

    if (-not (Test-Path $screenshotPath) -or (Get-Item $screenshotPath).Length -le 0) {
        throw "The surface trial screenshot was not written: $screenshotPath"
    }

    Write-Host "Dungeon Level 5 surface trial visual smoke passed: screenshot saved to $screenshotPath"
}
finally {
    if ($unreal -and -not $unreal.HasExited) {
        Wait-Process -Id $unreal.Id -Timeout 12 -ErrorAction SilentlyContinue
        if (-not $unreal.HasExited) {
            Stop-Process -Id $unreal.Id -Force -ErrorAction SilentlyContinue
        }
    }
}

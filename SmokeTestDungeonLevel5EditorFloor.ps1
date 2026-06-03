$ErrorActionPreference = "Stop"

$workspace = Split-Path -Parent $MyInvocation.MyCommand.Path
$project = Join-Path $workspace "DungeonBrood.uproject"
$logPath = Join-Path $workspace "Saved\Logs\DungeonBrood.log"
Remove-Item -Path $logPath -Force -ErrorAction SilentlyContinue

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
        "-ResX=960",
        "-ResY=540",
        "-WinX=160",
        "-WinY=160"
    )
    $unreal = Start-Process -FilePath $unrealEditor -ArgumentList $arguments -WorkingDirectory $workspace -PassThru

    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_EDITOR_AUTHORED_MAP_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_EDITOR_FLOOR_COLLISION_READY"

    Write-Host "Dungeon Level 5 editor floor smoke passed: player stayed above the saved editor map floor."
}
finally {
    if ($unreal -and -not $unreal.HasExited) {
        Stop-Process -Id $unreal.Id -Force -ErrorAction SilentlyContinue
    }
}

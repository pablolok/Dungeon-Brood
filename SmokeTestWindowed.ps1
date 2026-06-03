$ErrorActionPreference = "Stop"

$workspace = Split-Path -Parent $MyInvocation.MyCommand.Path
$logPath = Join-Path $workspace "Saved\Logs\DungeonBrood.log"
$screenshotPath = Join-Path $workspace "Saved\Automation\WindowedSmokeViewport.png"
Remove-Item -Path $logPath -Force -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Path (Split-Path -Parent $screenshotPath) -Force | Out-Null
Remove-Item -Path $screenshotPath -Force -ErrorAction SilentlyContinue
$existingProcessIds = @(
    Get-CimInstance Win32_Process -Filter "Name='UnrealEditor.exe'" |
        Select-Object -ExpandProperty ProcessId
)

$launcher = $null
$unreal = $null

function Wait-LogPattern {
    param(
        [string] $Pattern,
        [int] $TimeoutSeconds = 45
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

function Wait-LogAnyPattern {
    param(
        [string[]] $Patterns,
        [int] $TimeoutSeconds = 45
    )

    $deadline = (Get-Date).AddSeconds($TimeoutSeconds)
    while ((Get-Date) -lt $deadline) {
        if (Test-Path $logPath) {
            $content = Get-Content -Path $logPath -Raw
            foreach ($pattern in $Patterns) {
                if ($content -match $pattern) {
                    return
                }
            }
        }

        Start-Sleep -Milliseconds 500
    }

    throw "Timed out waiting for any log pattern: $($Patterns -join ', ')"
}

Add-Type @"
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

public static class BroodWindowFinder
{
    public delegate bool EnumWindowsProc(IntPtr hWnd, IntPtr lParam);

    [DllImport("user32.dll")]
    public static extern bool EnumWindows(EnumWindowsProc lpEnumFunc, IntPtr lParam);

    [DllImport("user32.dll")]
    public static extern bool IsWindowVisible(IntPtr hWnd);

    [DllImport("user32.dll")]
    public static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint processId);

    public static IntPtr FindVisibleWindowForProcess(uint targetProcessId)
    {
        IntPtr result = IntPtr.Zero;
        EnumWindows(delegate(IntPtr hWnd, IntPtr lParam) {
            uint processId;
            GetWindowThreadProcessId(hWnd, out processId);
            if (processId == targetProcessId && IsWindowVisible(hWnd)) {
                result = hWnd;
                return false;
            }
            return true;
        }, IntPtr.Zero);
        return result;
    }
}
"@

function Wait-GameWindowHandle {
    param(
        [int] $ProcessId,
        [int] $TimeoutSeconds = 45
    )

    $deadline = (Get-Date).AddSeconds($TimeoutSeconds)
    while ((Get-Date) -lt $deadline) {
        $windowHandle = [BroodWindowFinder]::FindVisibleWindowForProcess([uint32] $ProcessId)
        if ($windowHandle -and $windowHandle -ne [IntPtr]::Zero) {
            return $windowHandle
        }

        $process = Get-Process -Id $ProcessId -ErrorAction SilentlyContinue
        if ($process) {
            $process.Refresh()
            if ($process.MainWindowHandle -and $process.MainWindowHandle -ne [IntPtr]::Zero) {
                return $process.MainWindowHandle
            }
        }

        Start-Sleep -Milliseconds 500
    }

    throw "DungeonBrood did not expose a real game window."
}

try {
    $launcher = Start-Process -FilePath "cmd.exe" -ArgumentList "/c", "RunGame.bat -ExecCmds=""BroodCaptureWindowedSmokeScreenshot""" -WorkingDirectory $workspace -PassThru
    Start-Sleep -Seconds 10

    $unreal = Get-CimInstance Win32_Process -Filter "Name='UnrealEditor.exe'" |
        Where-Object {
            $existingProcessIds -notcontains $_.ProcessId -and
            $_.CommandLine -like "*DungeonBrood.uproject*"
        } |
        Select-Object -First 1

    if (-not $unreal) {
        throw "UnrealEditor process was not found after RunGame.bat."
    }

    $windowHandle = Wait-GameWindowHandle -ProcessId $unreal.ProcessId

    if ($unreal.CommandLine -notlike "*-windowed*" -or
        $unreal.CommandLine -notlike "*-ResX=1280*" -or
        $unreal.CommandLine -notlike "*-ResY=720*") {
        throw "DungeonBrood was not launched with the expected windowed flags."
    }

    Wait-LogPattern "BROOD_IMPORTED_WORLD_ASSETS_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_MOOD_LIGHTING_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_READABLE_DARKNESS_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_MODULAR_FLOOR_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_ROUTE_LAYOUT_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_LOW_PROFILE_ROUTE_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_ROUTE_COLLISION_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_ART_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_PROLOGUE_DRESSING_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_TRAVERSAL_COMPOSITION_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_CREEPY_DRESSING_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_JAILER_APPROACH_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_JAILER_BOSS_ROOM_AUTHORED_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_EXIT_ARCHITECTURE_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_EXIT_DESCENT_ROOM_AUTHORED_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_BROKEN_THRESHOLD_READY"
    Wait-LogAnyPattern @("BROOD_DUNGEON_LEVEL5_ENEMY_DRESSING_READY", "BROOD_DUNGEON_LEVEL5_ENEMY_DRESSING_FALLBACK_READY")
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_CREATURE_VARIETY_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_ENEMY_SILHOUETTE_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_ENEMY_MOTION_READY"
    Wait-LogPattern "BROOD_VILLAGE_CRYPT_DRESSING_READY"
    Wait-LogPattern "BROOD_LARVA_FALLBACK_READY"
    Wait-LogPattern "BROOD_LARVA_GLOW_LIGHT_READY"
    Wait-LogPattern "BROOD_LARVA_ALIEN_SILHOUETTE_READY"
    Wait-LogPattern "BROOD_LARVA_ORGANIC_MOTION_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_CAMERA_READABILITY_READY"
    Wait-LogPattern "BROOD_IMPORTED_ENEMY_ASSET_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_CLEAN_FIRST_LOOK_READY"
    Wait-LogPattern "BROOD_DUNGEON_LEVEL5_ART_REVIEW_NAMEPLATES_HIDDEN_READY"
    Wait-LogPattern "BROOD_WINDOWED_VIEWPORT_SCREENSHOT_REQUESTED"
    Wait-LogPattern "BROOD_MOUSE_AIM_ENABLED"
    Wait-LogPattern "Engine is initialized"
    Start-Sleep -Seconds 1

    $shell = New-Object -ComObject WScript.Shell
    [void] $shell.AppActivate([int] $unreal.ProcessId)
    Start-Sleep -Milliseconds 500

    Add-Type @"
using System;
using System.Runtime.InteropServices;

public static class MouseInput
{
    [StructLayout(LayoutKind.Sequential)]
    public struct RECT
    {
        public int Left;
        public int Top;
        public int Right;
        public int Bottom;
    }

    [DllImport("user32.dll")]
    public static extern bool GetWindowRect(IntPtr hWnd, out RECT rect);

    [DllImport("user32.dll")]
    public static extern bool SetCursorPos(int x, int y);

    [DllImport("user32.dll")]
    public static extern void mouse_event(uint flags, uint dx, uint dy, uint data, UIntPtr extraInfo);

    [DllImport("user32.dll")]
    public static extern bool SetForegroundWindow(IntPtr hWnd);

    [DllImport("user32.dll")]
    public static extern void keybd_event(byte virtualKey, byte scanCode, uint flags, UIntPtr extraInfo);
}
"@

    $windowRect = New-Object MouseInput+RECT
    if (-not [MouseInput]::GetWindowRect($windowHandle, [ref] $windowRect)) {
        Add-Type -AssemblyName System.Windows.Forms
        $screenBounds = [System.Windows.Forms.Screen]::PrimaryScreen.Bounds
        $windowRect.Left = $screenBounds.Left
        $windowRect.Top = $screenBounds.Top
        $windowRect.Right = $screenBounds.Right
        $windowRect.Bottom = $screenBounds.Bottom
    }

    $clickX = [int] ($windowRect.Left + (($windowRect.Right - $windowRect.Left) * 0.70))
    $clickY = [int] ($windowRect.Top + (($windowRect.Bottom - $windowRect.Top) * 0.55))
    [void] [MouseInput]::SetForegroundWindow($windowHandle)
    [void] $shell.AppActivate([int] $unreal.ProcessId)
    Start-Sleep -Milliseconds 500
    [void] [MouseInput]::SetCursorPos($clickX, $clickY)
    for ($attackIndex = 0; $attackIndex -lt 3; $attackIndex++) {
        [MouseInput]::mouse_event(0x0002, 0, 0, 0, [UIntPtr]::Zero)
        Start-Sleep -Milliseconds 140
        [MouseInput]::mouse_event(0x0004, 0, 0, 0, [UIntPtr]::Zero)
        Start-Sleep -Milliseconds 350
    }
    for ($attackIndex = 0; $attackIndex -lt 2; $attackIndex++) {
        [MouseInput]::keybd_event(0x20, 0, 0, [UIntPtr]::Zero)
        Start-Sleep -Milliseconds 140
        [MouseInput]::keybd_event(0x20, 0, 0x0002, [UIntPtr]::Zero)
        Start-Sleep -Milliseconds 350
    }
    Start-Sleep -Seconds 1
    [void] [MouseInput]::SetForegroundWindow($windowHandle)
    [void] $shell.AppActivate([int] $unreal.ProcessId)
    Start-Sleep -Milliseconds 250
    [MouseInput]::mouse_event(0x0008, 0, 0, 0, [UIntPtr]::Zero)
    [void] [MouseInput]::SetCursorPos($clickX + 120, $clickY)
    Start-Sleep -Milliseconds 500
    [MouseInput]::mouse_event(0x0010, 0, 0, 0, [UIntPtr]::Zero)
    Start-Sleep -Seconds 1

    [void] [MouseInput]::SetForegroundWindow($windowHandle)
    Start-Sleep -Milliseconds 250
    [MouseInput]::keybd_event(0x1B, 0, 0, [UIntPtr]::Zero)
    [MouseInput]::keybd_event(0x1B, 0, 0x0002, [UIntPtr]::Zero)
    Start-Sleep -Seconds 2
    [void] [MouseInput]::SetForegroundWindow($windowHandle)
    Start-Sleep -Milliseconds 250
    [MouseInput]::keybd_event(0x1B, 0, 0, [UIntPtr]::Zero)
    [MouseInput]::keybd_event(0x1B, 0, 0x0002, [UIntPtr]::Zero)
    Start-Sleep -Seconds 2

    if (-not (Test-Path $screenshotPath) -or (Get-Item $screenshotPath).Length -le 0) {
        throw "The Unreal viewport screenshot was not written: $screenshotPath"
    }

    $log = Get-Content -Path $logPath -Raw
    if ($log -notmatch "Brood pause menu shown\." -or $log -notmatch "Brood pause menu hidden\.") {
        throw "ESC did not show and hide the pause menu according to the runtime log."
    }
    if ($log -notmatch "BROOD_MOUSE_AIM_ENABLED" -or $log -notmatch "Brood attack (hit|missed)") {
        throw "The visible gameplay cursor or the real left click attack was not observed in the runtime log."
    }
    if ($log -notmatch "BROOD_CAMERA_ORBIT_STARTED" -or $log -notmatch "BROOD_CAMERA_ORBIT_ENDED") {
        throw "The real right mouse drag did not activate camera orbit according to the runtime log."
    }
    if ($log -notmatch "BROOD_WORLD_DRESSING_READY") {
        throw "The dungeon world dressing marker was not observed in the runtime log."
    }
    if ($log -notmatch "BROOD_DUNGEON_LEVEL5_MODULAR_FLOOR_READY") {
        throw "The Dungeon Level 5 modular floor marker was not observed in the runtime log."
    }
    if ($log -notmatch "BROOD_DUNGEON_LEVEL5_ROUTE_LAYOUT_READY") {
        throw "The Dungeon Level 5 route layout marker was not observed in the runtime log."
    }
    if ($log -notmatch "BROOD_DUNGEON_LEVEL5_MOOD_LIGHTING_READY" -or
        $log -notmatch "BROOD_DUNGEON_LEVEL5_READABLE_DARKNESS_READY" -or
        $log -notmatch "BROOD_DUNGEON_LEVEL5_LOW_PROFILE_ROUTE_READY") {
        throw "The Dungeon Level 5 mood lighting or low-profile route marker was not observed in the runtime log."
    }
    if ($log -notmatch "BROOD_DUNGEON_LEVEL5_ROUTE_COLLISION_READY") {
        throw "The Dungeon Level 5 route collision marker was not observed in the runtime log."
    }
    if ($log -notmatch "BROOD_DUNGEON_LEVEL5_ART_READY") {
        throw "The Dungeon Level 5 art marker was not observed in the runtime log."
    }
    if ($log -notmatch "BROOD_DUNGEON_LEVEL5_PROLOGUE_DRESSING_READY") {
        throw "The Dungeon Level 5 prologue dressing marker was not observed in the runtime log."
    }
    if ($log -notmatch "BROOD_DUNGEON_LEVEL5_TRAVERSAL_COMPOSITION_READY") {
        throw "The Dungeon Level 5 traversal composition marker was not observed in the runtime log."
    }
    if ($log -notmatch "BROOD_DUNGEON_LEVEL5_CREEPY_DRESSING_READY") {
        throw "The Dungeon Level 5 creepy dressing marker was not observed in the runtime log."
    }
    if ($log -notmatch "BROOD_DUNGEON_LEVEL5_JAILER_APPROACH_READY") {
        throw "The Dungeon Level 5 Jailer approach dressing marker was not observed in the runtime log."
    }
    if ($log -notmatch "BROOD_DUNGEON_LEVEL5_EXIT_ARCHITECTURE_READY") {
        throw "The Dungeon Level 5 lower exit architecture marker was not observed in the runtime log."
    }
    if ($log -notmatch "BROOD_DUNGEON_LEVEL5_BROKEN_THRESHOLD_READY") {
        throw "The Dungeon Level 5 Broken Threshold dressing marker was not observed in the runtime log."
    }
    if ($log -notmatch "BROOD_DUNGEON_LEVEL5_ENEMY_DRESSING_READY" -and
        $log -notmatch "BROOD_DUNGEON_LEVEL5_ENEMY_DRESSING_FALLBACK_READY") {
        throw "The Dungeon Level 5 enemy dressing marker was not observed in the runtime log."
    }
    if ($log -notmatch "BROOD_DUNGEON_LEVEL5_CREATURE_VARIETY_READY") {
        throw "The Dungeon Level 5 creature variety marker was not observed in the runtime log."
    }
    if ($log -notmatch "BROOD_DUNGEON_LEVEL5_ENEMY_SILHOUETTE_READY") {
        throw "The Dungeon Level 5 enemy silhouette marker was not observed in the runtime log."
    }
    if ($log -notmatch "BROOD_DUNGEON_LEVEL5_ENEMY_MOTION_READY") {
        throw "The Dungeon Level 5 enemy motion marker was not observed in the runtime log."
    }
    if ($log -notmatch "BROOD_VILLAGE_CRYPT_DRESSING_READY") {
        throw "The village crypt dressing marker was not observed in the runtime log."
    }
    if ($log -notmatch "BROOD_LARVA_GLOW_LIGHT_READY") {
        throw "The larva glow light marker was not observed in the runtime log."
    }
    if ($log -notmatch "BROOD_LARVA_ALIEN_SILHOUETTE_READY" -or
        $log -notmatch "BROOD_LARVA_ORGANIC_MOTION_READY") {
        throw "The advanced larva alien silhouette or organic motion marker was not observed in the runtime log."
    }
    if ($log -notmatch "BROOD_DUNGEON_LEVEL5_CAMERA_READABILITY_READY") {
        throw "The Dungeon Level 5 camera readability marker was not observed in the runtime log."
    }
    if ($log -notmatch "BROOD_DUNGEON_LEVEL5_CLEAN_FIRST_LOOK_READY") {
        throw "The clean first-look screenshot marker was not observed in the runtime log."
    }
    if ($log -notmatch "BROOD_DUNGEON_LEVEL5_ART_REVIEW_NAMEPLATES_HIDDEN_READY") {
        throw "The art-review nameplate hiding marker was not observed in the runtime log."
    }
    if ($log -notmatch "BROOD_IMPORTED_WORLD_ASSETS_READY" -or
        $log -notmatch "BROOD_LARVA_FALLBACK_READY" -or
        $log -notmatch "BROOD_IMPORTED_ENEMY_ASSET_READY") {
        throw "Expected world, larva player fallback, or enemy asset markers were not observed in the runtime log."
    }

    Write-Host "Windowed smoke test passed: real window, screenshot saved to $screenshotPath, larva player fallback, imported world/enemy assets, Dungeon Level 5 enemy dressing, village crypt dressing, 1280x720 flags, real attack input, right mouse camera orbit, ESC show/hide."
}
finally {
    if ($unreal) {
        Wait-Process -Id $unreal.ProcessId -Timeout 12 -ErrorAction SilentlyContinue
        if (Get-Process -Id $unreal.ProcessId -ErrorAction SilentlyContinue) {
            Stop-Process -Id $unreal.ProcessId -Force -ErrorAction SilentlyContinue
        }
    }

    if ($launcher) {
        Wait-Process -Id $launcher.Id -Timeout 3 -ErrorAction SilentlyContinue
        if (-not $launcher.HasExited) {
            Stop-Process -Id $launcher.Id -Force -ErrorAction SilentlyContinue
        }
    }
}

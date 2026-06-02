$ErrorActionPreference = "Stop"

$workspace = Split-Path -Parent $MyInvocation.MyCommand.Path
$logPath = Join-Path $workspace "Saved\Logs\DungeonBrood.log"
Remove-Item -Path $logPath -Force -ErrorAction SilentlyContinue
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
    $launcher = Start-Process -FilePath "cmd.exe" -ArgumentList "/c", "RunGame.bat" -WorkingDirectory $workspace -PassThru
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
    Wait-LogPattern "BROOD_IMPORTED_PLAYER_ASSET_READY"
    Wait-LogPattern "BROOD_IMPORTED_ENEMY_ASSET_READY"
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
    [void] [MouseInput]::SetCursorPos($clickX, $clickY)
    [MouseInput]::mouse_event(0x0002, 0, 0, 0, [UIntPtr]::Zero)
    [MouseInput]::mouse_event(0x0004, 0, 0, 0, [UIntPtr]::Zero)
    Start-Sleep -Seconds 1
    [MouseInput]::mouse_event(0x0008, 0, 0, 0, [UIntPtr]::Zero)
    [void] [MouseInput]::SetCursorPos($clickX + 120, $clickY)
    Start-Sleep -Milliseconds 500
    [MouseInput]::mouse_event(0x0010, 0, 0, 0, [UIntPtr]::Zero)
    Start-Sleep -Seconds 1

    $shell.SendKeys("{ESC}")
    Start-Sleep -Seconds 2
    [void] $shell.AppActivate([int] $unreal.ProcessId)
    $shell.SendKeys("{ESC}")
    Start-Sleep -Seconds 2

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
    if ($log -notmatch "BROOD_IMPORTED_WORLD_ASSETS_READY" -or
        $log -notmatch "BROOD_IMPORTED_PLAYER_ASSET_READY" -or
        $log -notmatch "BROOD_IMPORTED_ENEMY_ASSET_READY") {
        throw "Imported Fab world/player/enemy asset markers were not observed in the runtime log."
    }

    Write-Host "Windowed smoke test passed: real window, imported Fab assets, dungeon dressing, 1280x720 flags, real left click attack, right mouse camera orbit, ESC show/hide."
}
finally {
    if ($unreal) {
        Stop-Process -Id $unreal.ProcessId -Force -ErrorAction SilentlyContinue
    }

    if ($launcher) {
        Stop-Process -Id $launcher.Id -Force -ErrorAction SilentlyContinue
    }
}

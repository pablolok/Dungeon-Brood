@echo off
setlocal

set "PROJECT=%~dp0DungeonBrood.uproject"
set "REQUESTED_UE_ROOT=%UE_ROOT%"
set "UE_ROOT="

if not "%REQUESTED_UE_ROOT%"=="" if exist "%REQUESTED_UE_ROOT%\Engine\Binaries\Win64\UnrealEditor.exe" set "UE_ROOT=%REQUESTED_UE_ROOT%"
if "%UE_ROOT%"=="" if defined UE_5_7_ROOT if exist "%UE_5_7_ROOT%\Engine\Binaries\Win64\UnrealEditor.exe" set "UE_ROOT=%UE_5_7_ROOT%"
if "%UE_ROOT%"=="" if exist "C:\Program Files\Epic Games\UE_5.7.4\Engine\Binaries\Win64\UnrealEditor.exe" set "UE_ROOT=C:\Program Files\Epic Games\UE_5.7.4"
if "%UE_ROOT%"=="" if exist "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe" set "UE_ROOT=C:\Program Files\Epic Games\UE_5.7"
if "%UE_ROOT%"=="" if exist "E:\EpicGames\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe" set "UE_ROOT=E:\EpicGames\UE_5.7"
if "%UE_ROOT%"=="" for /f "usebackq delims=" %%I in (`powershell -NoProfile -ExecutionPolicy Bypass -Command "$tool='Engine\Binaries\Win64\UnrealEditor.exe'; $m='C:\ProgramData\Epic\EpicGamesLauncher\Data\Manifests'; if(Test-Path $m){Get-ChildItem $m -Filter *.item | ForEach-Object{try{$j=Get-Content -Raw $_.FullName|ConvertFrom-Json; $p=$j.InstallLocation; if($p -and (Test-Path (Join-Path $p $tool))){$p}}catch{}} | Sort-Object @{Expression={if($_ -match 'UE_5\.7'){0}else{1}}}, @{Expression={$_};Descending=$true} | Select-Object -First 1}"`) do set "UE_ROOT=%%I"

if "%UE_ROOT%"=="" (
    echo ERROR: Unreal Engine was not found.
    exit /b 1
)

echo Running Deepborn Dungeon Level 5 surface trial smoke test with "%UE_ROOT%"
"%UE_ROOT%\Engine\Binaries\Win64\UnrealEditor.exe" "%PROJECT%" -game -nullrhi -nosound -unattended -ExecCmds="BroodDungeonLevel5SurfaceTrialSmokeTest"
if errorlevel 1 (
    echo.
    echo ERROR: Dungeon Level 5 surface trial smoke test failed.
    exit /b 1
)

powershell -NoProfile -ExecutionPolicy Bypass -Command "$log=Get-Content -Raw '%~dp0Saved\Logs\DungeonBrood.log'; if($log -notmatch 'BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_SMOKE_STARTED' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_SURFACE_TEMPTATION_READY' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_ENTERED' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_GUARDS_READY' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_HUMANOID_SILHOUETTES_READY' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_DEATH_READY' -or $log -notmatch 'THIS_BODY_WAS_NOT_READY' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_SMOKE_PASSED' -or $log -match 'BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_SMOKE_FAILED'){exit 1}"
if errorlevel 1 (
    echo.
    echo ERROR: Dungeon Level 5 surface trial runtime markers were not successful.
    exit /b 1
)

endlocal

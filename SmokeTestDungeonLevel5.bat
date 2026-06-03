@echo off
setlocal

set "PROJECT=%~dp0DungeonBrood.uproject"
set "REQUESTED_UE_ROOT=%UE_ROOT%"
set "UE_ROOT="

if not "%REQUESTED_UE_ROOT%"=="" if exist "%REQUESTED_UE_ROOT%\Engine\Binaries\Win64\UnrealEditor.exe" set "UE_ROOT=%REQUESTED_UE_ROOT%"
if "%UE_ROOT%"=="" if defined UE_5_7_ROOT if exist "%UE_5_7_ROOT%\Engine\Binaries\Win64\UnrealEditor.exe" set "UE_ROOT=%UE_5_7_ROOT%"
if "%UE_ROOT%"=="" if exist "C:\Program Files\Epic Games\UE_5.7.4\Engine\Binaries\Win64\UnrealEditor.exe" set "UE_ROOT=C:\Program Files\Epic Games\UE_5.7.4"
if "%UE_ROOT%"=="" if exist "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe" set "UE_ROOT=C:\Program Files\Epic Games\UE_5.7"
if "%UE_ROOT%"=="" for /f "usebackq delims=" %%I in (`powershell -NoProfile -ExecutionPolicy Bypass -Command "$tool='Engine\Binaries\Win64\UnrealEditor.exe'; $m='C:\ProgramData\Epic\EpicGamesLauncher\Data\Manifests'; if(Test-Path $m){Get-ChildItem $m -Filter *.item | ForEach-Object{try{$j=Get-Content -Raw $_.FullName|ConvertFrom-Json; $p=$j.InstallLocation; if($p -and (Test-Path (Join-Path $p $tool))){$p}}catch{}} | Sort-Object @{Expression={if($_ -match 'UE_5\.7'){0}else{1}}}, @{Expression={$_};Descending=$true} | Select-Object -First 1}"`) do set "UE_ROOT=%%I"

if "%UE_ROOT%"=="" (
    echo ERROR: Unreal Engine was not found.
    exit /b 1
)

echo Running Deepborn Dungeon Level 5 smoke test with "%UE_ROOT%"
"%UE_ROOT%\Engine\Binaries\Win64\UnrealEditor.exe" "%PROJECT%" -game -nullrhi -nosound -unattended -ExecCmds="BroodDungeonLevel5SmokeTest"
if errorlevel 1 (
    echo.
    echo ERROR: Dungeon Level 5 smoke test failed.
    exit /b 1
)

powershell -NoProfile -ExecutionPolicy Bypass -Command "$log=Get-Content -Raw '%~dp0Saved\Logs\DungeonBrood.log'; if($log -notmatch 'BROOD_DUNGEON_LEVEL5_SMOKE_STARTED' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_PROLOGUE_DRESSING_READY' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_TRAVERSAL_COMPOSITION_READY' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_CREEPY_DRESSING_READY' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_JAILER_APPROACH_READY' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_JAILER_BOSS_ROOM_AUTHORED_READY' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_EXIT_ARCHITECTURE_READY' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_EXIT_DESCENT_ROOM_AUTHORED_READY' -or $log -notmatch 'BROOD_LARVA_ALIEN_SILHOUETTE_READY' -or $log -notmatch 'BROOD_LARVA_ORGANIC_MOTION_READY' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_EVOLUTION_SELECTED' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_BOSS_SEEN' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_JAILER_HUSK_SEEN' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_JAILER_HUSK_DRESSING_READY|BROOD_DUNGEON_LEVEL5_JAILER_HUSK_FALLBACK_READY' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_JAILER_HUSK_BESPOKE_SILHOUETTE_READY' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_JAILER_HUSK_PLACEHOLDER_BODY_HIDDEN_READY' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_JAILER_HUSK_VERTICAL_BODY_READY' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_JAILER_HUSK_CHAIN_MOTION_READY' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_CREATURE_VARIETY_READY' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_ENEMY_SILHOUETTE_READY' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_ENEMY_MOTION_READY' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_READABLE_DARKNESS_READY' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_EXIT_UNLOCKED' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_EXIT_UNLOCKED_ASSET_FEEDBACK_READY' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_COMPLETE' -or $log -notmatch 'BROOD_DUNGEON_LEVEL5_SMOKE_PASSED' -or $log -match 'BROOD_DUNGEON_LEVEL5_SMOKE_FAILED'){exit 1}"
if errorlevel 1 (
    echo.
    echo ERROR: Dungeon Level 5 smoke test runtime marker was not successful.
    exit /b 1
)

endlocal

@echo off
setlocal

set "PROJECT=%~dp0DungeonBrood.uproject"
set "UE_ROOT="

if not "%UE_5_3_ROOT%"=="" if exist "%UE_5_3_ROOT%\Engine\Build\BatchFiles\GenerateProjectFiles.bat" set "UE_ROOT=%UE_5_3_ROOT%"
if "%UE_ROOT%"=="" if exist "C:\Program Files\Epic Games\UE_5.3\Engine\Build\BatchFiles\GenerateProjectFiles.bat" set "UE_ROOT=C:\Program Files\Epic Games\UE_5.3"
if "%UE_ROOT%"=="" if exist "C:\Program Files\Epic Games\UE_5.3EA\Engine\Build\BatchFiles\GenerateProjectFiles.bat" set "UE_ROOT=C:\Program Files\Epic Games\UE_5.3EA"
if "%UE_ROOT%"=="" if exist "D:\Epic Games\UE_5.3\Engine\Build\BatchFiles\GenerateProjectFiles.bat" set "UE_ROOT=D:\Epic Games\UE_5.3"
if "%UE_ROOT%"=="" if exist "D:\Program Files\Epic Games\UE_5.3\Engine\Build\BatchFiles\GenerateProjectFiles.bat" set "UE_ROOT=D:\Program Files\Epic Games\UE_5.3"
if "%UE_ROOT%"=="" if exist "E:\Epic Games\UE_5.3\Engine\Build\BatchFiles\GenerateProjectFiles.bat" set "UE_ROOT=E:\Epic Games\UE_5.3"
if "%UE_ROOT%"=="" if exist "E:\Program Files\Epic Games\UE_5.3\Engine\Build\BatchFiles\GenerateProjectFiles.bat" set "UE_ROOT=E:\Program Files\Epic Games\UE_5.3"
if "%UE_ROOT%"=="" for /f "usebackq delims=" %%I in (`powershell -NoProfile -ExecutionPolicy Bypass -Command "$m='C:\ProgramData\Epic\EpicGamesLauncher\Data\Manifests'; if(Test-Path $m){Get-ChildItem $m -Filter *.item | ForEach-Object{try{$j=Get-Content -Raw $_.FullName|ConvertFrom-Json; $p=$j.InstallLocation; if($p -and (Test-Path (Join-Path $p 'Engine\Build\BatchFiles\GenerateProjectFiles.bat'))){$p}}catch{}} | Select-Object -First 1}"`) do set "UE_ROOT=%%I"

if "%UE_ROOT%"=="" (
    echo ERROR: Unreal Engine 5.3 GenerateProjectFiles.bat was not found.
    echo Set UE_5_3_ROOT to your Unreal Engine 5.3 install folder and run again.
    pause
    exit /b 1
)

"%UE_ROOT%\Engine\Build\BatchFiles\GenerateProjectFiles.bat" -project="%PROJECT%" -game -engine
if errorlevel 1 (
    echo.
    echo ERROR: Project file generation failed.
    pause
    exit /b 1
)

endlocal

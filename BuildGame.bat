@echo off
setlocal

set "PROJECT=%~dp0DungeonBrood.uproject"
set "REQUESTED_UE_ROOT=%UE_ROOT%"
set "UE_ROOT="

if not "%REQUESTED_UE_ROOT%"=="" if exist "%REQUESTED_UE_ROOT%\Engine\Build\BatchFiles\Build.bat" set "UE_ROOT=%REQUESTED_UE_ROOT%"
if "%UE_ROOT%"=="" if defined UE_5_7_ROOT if exist "%UE_5_7_ROOT%\Engine\Build\BatchFiles\Build.bat" set "UE_ROOT=%UE_5_7_ROOT%"
if "%UE_ROOT%"=="" if defined UE_5_3_ROOT if exist "%UE_5_3_ROOT%\Engine\Build\BatchFiles\Build.bat" set "UE_ROOT=%UE_5_3_ROOT%"
if "%UE_ROOT%"=="" if exist "C:\Program Files\Epic Games\UE_5.7.4\Engine\Build\BatchFiles\Build.bat" set "UE_ROOT=C:\Program Files\Epic Games\UE_5.7.4"
if "%UE_ROOT%"=="" if exist "C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" set "UE_ROOT=C:\Program Files\Epic Games\UE_5.7"
if "%UE_ROOT%"=="" if exist "C:\Program Files\Epic Games\UE_5.7EA\Engine\Build\BatchFiles\Build.bat" set "UE_ROOT=C:\Program Files\Epic Games\UE_5.7EA"
if "%UE_ROOT%"=="" if exist "D:\Epic Games\UE_5.7.4\Engine\Build\BatchFiles\Build.bat" set "UE_ROOT=D:\Epic Games\UE_5.7.4"
if "%UE_ROOT%"=="" if exist "D:\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" set "UE_ROOT=D:\Epic Games\UE_5.7"
if "%UE_ROOT%"=="" if exist "D:\Program Files\Epic Games\UE_5.7.4\Engine\Build\BatchFiles\Build.bat" set "UE_ROOT=D:\Program Files\Epic Games\UE_5.7.4"
if "%UE_ROOT%"=="" if exist "D:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" set "UE_ROOT=D:\Program Files\Epic Games\UE_5.7"
if "%UE_ROOT%"=="" if exist "E:\Epic Games\UE_5.7.4\Engine\Build\BatchFiles\Build.bat" set "UE_ROOT=E:\Epic Games\UE_5.7.4"
if "%UE_ROOT%"=="" if exist "E:\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" set "UE_ROOT=E:\Epic Games\UE_5.7"
if "%UE_ROOT%"=="" if exist "E:\Program Files\Epic Games\UE_5.7.4\Engine\Build\BatchFiles\Build.bat" set "UE_ROOT=E:\Program Files\Epic Games\UE_5.7.4"
if "%UE_ROOT%"=="" if exist "E:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" set "UE_ROOT=E:\Program Files\Epic Games\UE_5.7"
if "%UE_ROOT%"=="" if exist "C:\Program Files\Epic Games\UE_5.3\Engine\Build\BatchFiles\Build.bat" set "UE_ROOT=C:\Program Files\Epic Games\UE_5.3"
if "%UE_ROOT%"=="" for /f "usebackq delims=" %%I in (`powershell -NoProfile -ExecutionPolicy Bypass -Command "$tool='Engine\Build\BatchFiles\Build.bat'; $m='C:\ProgramData\Epic\EpicGamesLauncher\Data\Manifests'; if(Test-Path $m){Get-ChildItem $m -Filter *.item | ForEach-Object{try{$j=Get-Content -Raw $_.FullName|ConvertFrom-Json; $p=$j.InstallLocation; if($p -and (Test-Path (Join-Path $p $tool))){$p}}catch{}} | Sort-Object @{Expression={if($_ -match 'UE_5\.7'){0}elseif($_ -match 'UE_5\.3'){1}else{2}}}, @{Expression={$_};Descending=$true} | Select-Object -First 1}"`) do set "UE_ROOT=%%I"

if "%UE_ROOT%"=="" (
    echo ERROR: Unreal Engine Build.bat was not found.
    echo Set UE_ROOT or UE_5_7_ROOT to your Unreal Engine 5.7 install folder and run again.
    pause
    exit /b 1
)

"%UE_ROOT%\Engine\Build\BatchFiles\Build.bat" DungeonBroodEditor Win64 Development -Project="%PROJECT%" -WaitMutex
if errorlevel 1 (
    echo.
    echo ERROR: Build failed.
    pause
    exit /b 1
)

endlocal

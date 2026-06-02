@echo off
setlocal

set "PROJECT=%~dp0DungeonBrood.uproject"
set "UE_ROOT="

if not "%UE_5_3_ROOT%"=="" if exist "%UE_5_3_ROOT%\Engine\Build\BatchFiles\Build.bat" set "UE_ROOT=%UE_5_3_ROOT%"
if "%UE_ROOT%"=="" if exist "C:\Program Files\Epic Games\UE_5.3\Engine\Build\BatchFiles\Build.bat" set "UE_ROOT=C:\Program Files\Epic Games\UE_5.3"
if "%UE_ROOT%"=="" if exist "C:\Program Files\Epic Games\UE_5.3EA\Engine\Build\BatchFiles\Build.bat" set "UE_ROOT=C:\Program Files\Epic Games\UE_5.3EA"

if "%UE_ROOT%"=="" (
    echo ERROR: Unreal Engine 5.3 Build.bat was not found.
    echo Set UE_5_3_ROOT to your Unreal Engine 5.3 install folder and run again.
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

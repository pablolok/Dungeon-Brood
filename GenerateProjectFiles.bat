@echo off
setlocal

set "PROJECT=%~dp0DungeonBrood.uproject"
set "UE_ROOT="

if not "%UE_5_3_ROOT%"=="" if exist "%UE_5_3_ROOT%\Engine\Build\BatchFiles\GenerateProjectFiles.bat" set "UE_ROOT=%UE_5_3_ROOT%"
if "%UE_ROOT%"=="" if exist "C:\Program Files\Epic Games\UE_5.3\Engine\Build\BatchFiles\GenerateProjectFiles.bat" set "UE_ROOT=C:\Program Files\Epic Games\UE_5.3"
if "%UE_ROOT%"=="" if exist "C:\Program Files\Epic Games\UE_5.3EA\Engine\Build\BatchFiles\GenerateProjectFiles.bat" set "UE_ROOT=C:\Program Files\Epic Games\UE_5.3EA"

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

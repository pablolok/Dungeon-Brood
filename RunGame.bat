@echo off
setlocal

set "PROJECT=%~dp0DungeonBrood.uproject"
set "UE_ROOT="

if not "%UE_5_3_ROOT%"=="" (
    if exist "%UE_5_3_ROOT%\Engine\Binaries\Win64\UnrealEditor.exe" (
        set "UE_ROOT=%UE_5_3_ROOT%"
    )
)

if "%UE_ROOT%"=="" (
    if exist "C:\Program Files\Epic Games\UE_5.3\Engine\Binaries\Win64\UnrealEditor.exe" (
        set "UE_ROOT=C:\Program Files\Epic Games\UE_5.3"
    )
)

if "%UE_ROOT%"=="" (
    if exist "C:\Program Files\Epic Games\UE_5.3EA\Engine\Binaries\Win64\UnrealEditor.exe" (
        set "UE_ROOT=C:\Program Files\Epic Games\UE_5.3EA"
    )
)

if "%UE_ROOT%"=="" (
    echo ERROR: Unreal Engine 5.3 was not found.
    echo Checked:
    echo   UE_5_3_ROOT
    echo   C:\Program Files\Epic Games\UE_5.3
    echo   C:\Program Files\Epic Games\UE_5.3EA
    echo.
    echo Set UE_5_3_ROOT to your Unreal Engine 5.3 install folder and run again.
    pause
    exit /b 1
)

if not exist "%PROJECT%" (
    echo ERROR: Project file not found: %PROJECT%
    pause
    exit /b 1
)

echo Launching Dungeon Brood with "%UE_ROOT%"
"%UE_ROOT%\Engine\Binaries\Win64\UnrealEditor.exe" "%PROJECT%" -game -log
if errorlevel 1 (
    echo.
    echo ERROR: UnrealEditor exited with an error.
    pause
    exit /b 1
)

endlocal

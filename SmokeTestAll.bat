@echo off
setlocal

call BuildGame.bat
if errorlevel 1 exit /b 1

call SmokeTestGame.bat
if errorlevel 1 exit /b 1

call SmokeTestCombat.bat
if errorlevel 1 exit /b 1

call SmokeTestWavePressure.bat
if errorlevel 1 exit /b 1

call SmokeTestMouseAim.bat
if errorlevel 1 exit /b 1

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0SmokeTestWindowed.ps1"
if errorlevel 1 exit /b 1

echo.
echo All Dungeon Brood smoke tests passed.
endlocal

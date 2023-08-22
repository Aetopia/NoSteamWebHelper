@echo off
set compiler=%1
if "%1"=="" set compiler=gcc.exe
cd "%~dp0"
"%compiler%" -O3 -municode -s -mwindows src/WinMain.c -o NoSteamWebHelper.exe
"%compiler%" -O3 -municode -s -shared src/DllMain.c -o NoSteamWebHelper.dll
upx --best --ultra-brute NoSteamWebHelper.exe NoSteamWebHelper.dll>nul 2>&1
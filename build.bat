@echo off
set compiler=%1
if "%1"=="" set compiler=gcc.exe
cd "%~dp0"
"%compiler%" -s -mwindows src/NoSteamWebHelper.c -lwtsapi32 -lwininet -o NoSteamWebHelper.exe
"%compiler%" -s -shared src/NoSteamWebHelperDll.c -lwtsapi32 -o NoSteamWebHelper.dll
upx --best --ultra-brute NoSteamWebHelper.exe NoSteamWebHelper.dll>nul 2>&1
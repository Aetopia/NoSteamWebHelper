@echo off
cd "%~dp0"
gcc -s -mwindows NoSteamWebHelper.c -lwtsapi32 -lshlwapi -o NoSteamWebHelper.exe
upx --best --ultra-brute NoSteamWebHelper.exe>nul 2>&1
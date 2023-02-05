@echo off
cd "%~dp0"
gcc -s -mwindows src/NoSteamWebHelper.c -lntdll -lDbghelp -lwtsapi32 -lshlwapi -o NoSteamWebHelper.exe
upx --best --ultra-brute NoSteamWebHelper.exe>nul 2>&1
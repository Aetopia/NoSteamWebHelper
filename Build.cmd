@echo off
cd "%~dp0"
i686-w64-mingw32-gcc.exe -s -nostdlib -mwindows WinMain.c -lKernel32 -o NoSteamWebHelper.exe
i686-w64-mingw32-gcc.exe -s -nostdlib -shared DllMain.c -lKernel32 -lUser32 -lAdvapi32 -o NoSteamWebHelper.dll
upx --best --ultra-brute dpapi.dll>nul 2>&1
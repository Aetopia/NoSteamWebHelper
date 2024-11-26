@echo off
cd "%~dp0">nul 2>&1
rmdir /Q /S "bin">nul 2>&1
mkdir "bin">nul 2>&1
i686-w64-mingw32-gcc.exe -Os -Wl,--gc-sections -municode -shared -nostdlib -s "DllMain.c" -lntdll -lWtsapi32 -lKernel32 -lUser32 -lAdvapi32 -lShell32 -o "bin\umpdc.dll"
upx.exe --best --ultra-brute "bin\*">nul 2>&1
@echo off
cd "%~dp0">nul 2>&1
rmdir /Q /S "bin">nul 2>&1
mkdir "bin">nul 2>&1

i686-w64-mingw32-gcc.exe -Os -Wl,--gc-sections -municode -shared -nostdlib -s "DllMain.c" -lntdll -lKernel32 -lUser32 -lAdvapi32 -lShell32 -o "bin\NoSteamWebHelper.dll"

i686-w64-mingw32-gcc.exe -Os -Wl,--gc-sections -municode -mwindows -nostdlib -s "WinMain.c" -lKernel32 -o "bin\NoSteamWebHelper.exe"

del "%TEMP%\.o">nul 2>&1
upx.exe --best --ultra-brute "bin\*">nul 2>&1
powershell.exe -Command "$ProgressPreference = 'SilentlyContinue'; Compress-Archive -Path 'bin\*' -DestinationPath 'bin\NoSteamWebHelper.zip' -Force">nul 2>&1
# NoSteamWebHelper
 A program that disables Steam's CEF/Chromium Embedded Framework.


## Aim
This program was created with the intent of replacing of Steam's command-line parameter `-no-browser` which was [recently removed.](https://steamcommunity.com/groups/SteamClientBeta/discussions/3/3710433479207750727/?ctp=42)


## How does NoSteamWebHelper kill or disable CEF/Chromium Embedded Framework? 

1. Launch Steam through `NoSteamWebHelper.exe`.
2. Wait for Steam's window to be hidden, this indicates Steam has fully initialized.
3. Suspend the single WebHelper process, this is acheived by passing `-cef-single-process -cef-disable-breakpad` to the steam executable.
4. Empty the WebHelper's working set, this will cause the memory usage of the process to go down to 0 MB.

# Usage
1. Download the latest release from [GitHub Releases](https://github.com/Aetopia/NoSteamWebHelper/releases).

2. Unzip `NoSteamWebHelper.zip`, place `NoSteamWebHelper.exe` & `NoSteamWebHelper.dll` in your Steam installation directory where `steam.exe` is located.

3. Make sure Steam is fully closed and run `NoSteamWebHelper.exe` to launch Steam without any WebHelper processes.
    > If you need to restore the WebHelper simply kill the suspended WebHelper process.

4. Enable Steam's Small Mode to view your Steam Library.

5. You may also pass command line arguments to `NoSteamWebHelper.exe` which will launch Steam with those command line arguments.

**Note: Steam's UI or features might break if you disable the WebHelper.**


# Build
1. Ensure you have a 32 Bit `GCC` installation and [`UPX`](https://upx.github.io/) for optional compression.
2. Use `build.bat` to compile the project, if your 32 bit GCC installation is located somewhere else on your system, you can use specify the `GCC` executable to use for compilation like this:   
    Syntax: `build.bat <Path to 32 bit GCC.exe>`         
    Example: `build.bat i686-w64-mingw32-gcc.exe`

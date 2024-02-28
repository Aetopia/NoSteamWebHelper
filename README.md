> [!IMPORTANT]
> As for 28th Feburary 2023, I am officially deprecating this project.<br>
> I have no interest in this project as well any desire to disable the CEF for Steam.<br>
> Feel free to fork and edit the source code to your needs.<br>
> I might do a redux of this project in the future, if I feel like it.

# NoSteamWebHelper
 A program that disables Steam's CEF/Chromium Embedded Framework.

## Aim
This program was created with the intent of replacing of Steam's command-line parameter `-no-browser` which was [recently removed.](https://steamcommunity.com/groups/SteamClientBeta/discussions/3/3710433479207750727/?ctp=42)


## How does NoSteamWebHelper kill or disable CEF/Chromium Embedded Framework? 

1. Launch Steam through `NoSteamWebHelper.exe`.
2. Monitor the following registry key: `HKEY_CURRENT_USER\SOFTWARE\Valve\Steam\Apps` and all of its subkeys.
3. When an app runs, suspend the window thread and kill any WebHelper processes.

# Usage
1. Download the latest release from [GitHub Releases](https://github.com/Aetopia/NoSteamWebHelper/releases).

2. Unzip `NoSteamWebHelper.zip`, place `NoSteamWebHelper.exe` & `NoSteamWebHelper.dll` in your Steam installation directory where `steam.exe` is located.

3. Make sure Steam is fully closed and run `NoSteamWebHelper.exe` to launch Steam.

4. The WebHelper processes will be terminated when an app runs but means you won't be able to access Steam's UI unless you exit the game.

# Build
1. Ensure you have a 32 Bit `GCC` installation and [`UPX`](https://upx.github.io/) for optional compression.
2. Use `build.bat` to compile the project, if your 32 bit GCC installation is located somewhere else on your system, you can use specify the `GCC` executable to use for compilation like this:   
    Syntax: `build.bat <Path to 32 bit GCC.exe>`         
    Example: `build.bat i686-w64-mingw32-gcc.exe`

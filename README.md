# NoSteamWebHelper
 A program that disables Steam's CEF/Chromium Embedded Framework.

## Aim
This program was created with the intent of replacing of Steam's command-line parameter `-no-browser` which was [removed.](https://steamcommunity.com/groups/SteamClientBeta/discussions/3/3710433479207750727/?ctp=42)


## How does NoSteamWebHelper kill or disable the CEF/Chromium Embedded Framework? 
- The executable does the following:
    - Launches Steam with the following arguments `-cef-single-process -cef-disable-breakpad` and appends any user provided ones, this restricts the CEF to a single process.
    - Injects the dynamic link library.

The dynamic link library toggles the CEF depending if an is app running or not.
- If an app is running then the CEF is disabled.
- If an app is not running then the CEF is enabled.

This way, the Steam UI is still accessible to use.
    

# Usage
1. Download the latest release from [GitHub Releases](https://github.com/Aetopia/NoSteamWebHelper/releases).

2. Unzip `NoSteamWebHelper.zip`, place `NoSteamWebHelper.exe` & `NoSteamWebHelper.dll` in your Steam installation directory where `steam.exe` is located.

3. Make sure Steam is fully closed and run `NoSteamWebHelper.exe` to launch Steam.

4. Start up an app and the CEF will be toggled accordingly.

# Build
1. Install [`GCC i686`](https://winlibs.com/) and [`UPX`](https://upx.github.io/) for optional compression.
2. Run `Build.cmd`.


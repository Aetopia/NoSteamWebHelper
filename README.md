# NoSteamWebHelper
 A program that disables Steam's CEF/Chromium Embedded Framework.

## Aim
This program was created with the intent of replacing of Steam's command-line parameter `-no-browser` which was [removed.](https://steamcommunity.com/groups/SteamClientBeta/discussions/3/3710433479207750727/?ctp=42)


## How does NoSteamWebHelper kill or disable the CEF/Chromium Embedded Framework? 
- The executable does the following:
    - Launches Steam with the following arguments `-cef-single-process -cef-disable-breakpad -cef-disable-gpu-compositing` and appends any user provided ones, this restricts the CEF to a single process.
    - Injects the dynamic link library.

The dynamic link library toggles the CEF depending if an is app running or not.
- If an app is running then the CEF is disabled.
- If an app is not running then the CEF is enabled.

> [!NOTE]
> You may also manually toggle the CEF via a tray icon provided by NoSteamWebHelper.

This way, the Steam UI is still accessible to use.
    

# Usage
1. Download the latest release from [GitHub Releases](https://github.com/Aetopia/NoSteamWebHelper/releases).

2. Unzip `NoSteamWebHelper.zip`, place `NoSteamWebHelper.exe` & `NoSteamWebHelper.dll` in your Steam installation directory where `steam.exe` is located.

3. Make sure Steam is fully closed and run `NoSteamWebHelper.exe` to launch Steam.

4. Start up an app and the CEF will be toggled accordingly.

# Build
1. Install [MSYS2](https://www.msys2.org/) & [UPX](https://upx.github.io/) for optional compression.
2. Update the MSYS2 Environment until there are no pending updates using:

    ```bash
    pacman -Syu --noconfirm
    ```

3. Install GCC i686 using:

    ```bash
    pacman -S mingw-w64-i686-gcc
    ```

4. Make sure `<MSYS2 Installation Directory>\mingw32\bin` is added to the Windows `PATH` environment variable.
5. Run [`Build.cmd`](Build.cmd).

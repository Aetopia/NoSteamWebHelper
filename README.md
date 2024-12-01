# NoSteamWebHelper
 A program that disables Steam's CEF/Chromium Embedded Framework.

## Aim
This program was created with the intent of replacing of Steam's command-line parameter `-no-browser` which was [removed.](https://steamcommunity.com/groups/SteamClientBeta/discussions/3/3710433479207750727/?ctp=42)


## How does NoSteamWebHelper kill or disable the CEF/Chromium Embedded Framework? 

The dynamic link library toggles the CEF depending if an is app running or not.

- If an app is running then the CEF is disabled.

- If an app is not running then the CEF is enabled.

This way, the Steam UI is still accessible to use.
    
# Usage
1. Download the latest release from [GitHub Releases](https://github.com/Aetopia/NoSteamWebHelper/releases).

2. Place `umpdc.dll` in your Steam installation directory where `steam.exe` is located.

3. Make sure Steam is fully closed and launch a new instance of Steam.

4. Start up an app and the CEF will be toggled accordingly.

> [!NOTE]
> - You may also manually toggle the CEF via a tray icon.
> - To prevent the CEF from automatically showing when restored, pass `-silent` to Steam.

# Build
1. Install [MSYS2](https://www.msys2.org/) & [UPX](https://upx.github.io/) for optional compression.
2. Update the MSYS2 Environment until there are no pending updates using:

    ```bash
    pacman -Syu --noconfirm
    ```

3. Install GCC i686 using:

    ```bash
    pacman -S mingw-w64-i686-gcc --noconfirm
    ```

4. Make sure `<MSYS2 Installation Directory>\mingw32\bin` is added to the Windows `PATH` environment variable.
5. Run [`Build.cmd`](Build.cmd).

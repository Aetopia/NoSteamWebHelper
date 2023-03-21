# NoSteamWebHelper
 A program that disables Steam's CEF/Chromium Embedded Framework.


## Aim
This program was created with the intent of replacing of Steam's command-line parameter `-no-browser` which was [recently removed.](https://steamcommunity.com/groups/SteamClientBeta/discussions/3/3710433479207750727/?ctp=42)


## How does NoSteamWebHelper kill or disable CEF/Chromium Embedded Framework? 

1. Launch Steam through `NoSteamWebHelper.exe`.
2. Wait for Steam's window to be hidden or shown, this indicates Steam has fully initialized.
3. Create a dummy DLL called `wininet.dll` that the Steam Webhelper loads.
4. Kill any WebHelper processes.
5. Any new WebHelper processes launched by Steam will simply not launch since it is using the newly created dummy DLL (`wininet.dll`).

# Usage
1. Download the latest release from [GitHub Releases](https://github.com/Aetopia/NoSteamWebHelper/releases).

2. Place `NoSteamWebHelper.exe` in your Steam installation directory where `steam.exe` is located.

3. Make sure Steam is fully closed and run `NoSteamWebHelper.exe` to launch Steam without any WebHelper processes.
    > If you need to restore the WebHelper simply go to `%STEAMPATH%\bin\cef\cef.win7x64` or `%STEAMPATH%\bin\cef\cef.win7` and delete `wininet.dll`/`wininet.dll.disabled`.  
    > **OR**     
    > Run `NoSteamWebHelper.exe` while Steam is running to restore the WebHelper.  

4. Enable Steam's Small Mode to view your Steam Library.

5. You may also pass command line arguments to `NoSteamWebHelper.exe` which will launch Steam with those command line arguments.

# Notes
1. **Steam's UI or features might break if you disable the WebHelper.**
2. **There is a slight CPU utilization penalty which can be circumvented by suspending the problematic thread.**
    - **[More Details Here.](https://gist.github.com/amitxv/0c454ba09a2feb3dd38f29c333f68c04?permalink_comment_id=4460606#gistcomment-4460606)**

# Build
1. Install `GCC` and [`UPX`](https://upx.github.io/) for optional compression.
2. Run `build.bat`.

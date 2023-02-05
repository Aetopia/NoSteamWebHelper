# NoSteamWebHelper
 A program that disables Steam's CEF/Chromium Embedded Framework by suspending the threads that invoke it.


## Aim
This program was created with the intent of replacing of Steam's command-line parameter `-no-browser` which was [recently removed.](https://steamcommunity.com/groups/SteamClientBeta/discussions/3/3710433479207750727/?ctp=42)

## Logic
**How does NoSteamWebHelper kill or disable CEF/Chromium Embedded Framework?**     

1. Launch a game through NoSteamWebHelper.
2. Find `steam.exe` and get a snapshot of its running threads.
3. Get the symbol table for each thread until we find anything called `CreateInterface`, this is the function used by Steam to interact with the WebHelper processes.
4. Suspend the threads that invoke `CreateInterface`.
5. Wait until the game launched exits.
6. Resume all threads that invoke `CreateInterface`.
7. Now the WebHelper has been restored.

# Usage
1. Download the latest release from [GitHub Releases.](https://github.com/Aetopia/NoSteamWebHelper/releases).

2. Open your game's launch options and use the following syntax:

    ```
    %Path to NoSteamWebHelper.exe% %COMMAND% %Process%
    ```
    Where: 
    - `%Path to NoSteamWebHelper.exe%` is the full path to `NoSteamWebHelper.exe`, you can use `Copy As Path` to get the full path of the executable.
    - `%COMMAND%` is the executable to be launched, the executable to be launched is passed by Steam.
    - `%Process%` is the actual game process. Provide the name of the actual game process if your game uses an anticheat or launcher. 

3. To use launch options:
    - Browse your game's local files.
    - Create a new file called `LaunchOptions.txt`.
    - Put in your launch options into `LaunchOptions.txt`.  
    - NoSteamWebHelper will automatically pass these to the program to be executed.

# Build
1. Install `GCC` and [`UPX`](https://upx.github.io/) for optional compression.
2. Run `build.bat`.
3. Use `NoSteamWebHelper.exe` with Steam.
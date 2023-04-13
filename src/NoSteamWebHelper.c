#include <windows.h>

int main(int argc, char *aArgv)
{
    MSG msg;
    int nWAppName, nCmdLine;
    WCHAR *wSteamArgs = L"steam.exe -silent -nofriendsui -no-dwrite -nointro -nobigpicture -nofasthtml -nocrashmonitor -noshaders -no-shared-textures -disablehighdpi -cef-single-process -cef-in-process-gpu -cef-disable-d3d11 -cef-disable-sandbox -disable-winh264 -no-cef-sandbox -vrdisable -cef-disable-breakpad -noverifyfiles -nobootstrapupdate -skipinitialbootstrap -norepairfiles -overridepackageurl \0",
          *wAppDir,
          *wCmdLine = 0,
          *wArgvStr = GetCommandLineW(),
          **wArgv = CommandLineToArgvW(wArgvStr, &argc),
          *dllName = L"NoSteamWebHelper.dll";
    SHELLEXECUTEINFOW seiw = {.cbSize = sizeof(SHELLEXECUTEINFOW),
                              .lpFile = L"steam.exe",
                              .fMask = SEE_MASK_NOCLOSEPROCESS,
                              .nShow = SW_NORMAL};
    LPVOID memory;

    nWAppName = wcslen(wArgv[0]) + 1;
    wAppDir = alloca(sizeof(WCHAR) * nWAppName);
    wcscpy(wAppDir, wArgv[0]);
    for (int i = nWAppName; i > 0; i--)
    {
        if (wAppDir[i] != L'\\')
            continue;
        wAppDir[i] = 0;
        SetCurrentDirectoryW(wAppDir);
        break;
    };

    nCmdLine = (wcslen(wArgvStr) - (nWAppName + 2));
    wCmdLine = alloca(sizeof(WCHAR) * (nCmdLine + wcslen(wSteamArgs) + 1));
    wcscpy(wCmdLine, wSteamArgs);
    wcscat(wCmdLine, wArgvStr + nWAppName + 2);
    seiw.lpParameters = wCmdLine;

    ShellExecuteExW(&seiw);
    memory = VirtualAllocEx(seiw.hProcess, NULL, sizeof(WCHAR) * 22, MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(seiw.hProcess, memory, dllName, sizeof(WCHAR) * 22, NULL);
    CreateRemoteThread(seiw.hProcess, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, memory, 0, 0);
    CloseHandle(seiw.hProcess);
    return 0;
}
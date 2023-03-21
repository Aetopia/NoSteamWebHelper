#include <windows.h>
#include <wtsapi32.h>
#include <shlwapi.h>

void WinEventProc(
    HWINEVENTHOOK hWinEventHook,
    DWORD event,
    HWND hWnd,
    LONG idObject,
    LONG idChild,
    DWORD idEventThread,
    DWORD dwmsEventTime)
{
    if ((event != EVENT_OBJECT_SHOW ||
         event != EVENT_OBJECT_HIDE) &&
        idObject != OBJID_WINDOW &&
        idChild != CHILDID_SELF)
        return;
    UnhookWinEvent(hWinEventHook);
    PostQuitMessage(0);
}

DWORD WaitForProcessThread(LPVOID lParam)
{
    WaitForSingleObject((HANDLE)lParam, INFINITE);
    ExitProcess(0);
    return TRUE;
}

int main()
{
    const WCHAR *dummyDll = L"bin\\cef\\cef.win7\\wininet.dll",
                *dummyDllx64 = L"bin\\cef\\cef.win7x64\\wininet.dll",
                *dummyDllDisabled = L"bin\\cef\\cef.win7\\wininet.dll.disabled",
                *dummyDllx64Disabled = L"bin\\cef\\cef.win7x64\\wininet.dll.disabled";

    MSG msg;
    WTS_PROCESS_INFOW *pWPI;
    DWORD count;
    int argc, nWAppName, nCmdLine;
    WCHAR *wAppDir,
        *wCmdLine = 0,
        *wArgvStr = GetCommandLineW(),
        **wArgv = CommandLineToArgvW(wArgvStr, &argc);
    SHELLEXECUTEINFOW seiw = {.cbSize = sizeof(SHELLEXECUTEINFOW),
                              .lpFile = L"steam.exe",
                              .fMask = SEE_MASK_NOCLOSEPROCESS,
                              .nShow = SW_NORMAL};
    HANDLE hProcess;

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

    if (argc != 1)
    {
        nCmdLine = (wcslen(wArgvStr) - (nWAppName + 2));
        wCmdLine = alloca(sizeof(WCHAR *) * nCmdLine);
        wcscpy(wCmdLine, wArgvStr + nWAppName + 2);
        seiw.lpParameters = wCmdLine;
    };

    if (PathFileExistsW(dummyDll) &&
        !MoveFileW(dummyDll, dummyDllDisabled))
        DeleteFileW(dummyDll);
    if (PathFileExistsW(dummyDllx64) &&
        !MoveFileW(dummyDllx64, dummyDllx64Disabled))
        DeleteFileW(dummyDllx64);

    ShellExecuteExW(&seiw);
    SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_HIDE, 0, WinEventProc, GetProcessId(seiw.hProcess), 0, WINEVENT_OUTOFCONTEXT);
    CreateThread(0, 0, WaitForProcessThread, (LPVOID)seiw.hProcess, 0, 0);
    while (GetMessageW(&msg, 0, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    };

    if (!MoveFileW(dummyDllDisabled, dummyDll))
        CloseHandle(CreateFileW(dummyDll, 0, 0, 0, CREATE_ALWAYS, 0, 0));
    if (!MoveFileW(dummyDllx64Disabled, dummyDllx64))
        CloseHandle(CreateFileW(dummyDllx64, 0, 0, 0, CREATE_ALWAYS, 0, 0));

    if (!WTSEnumerateProcessesW(WTS_CURRENT_SERVER, 0, 1, &pWPI, &count))
        return 1;

    for (DWORD i = 0; i < count; i++)
    {
        wcslwr(pWPI[i].pProcessName);
        if (wcscmp(pWPI[i].pProcessName, L"steamwebhelper.exe"))
            continue;
        hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pWPI[i].ProcessId);
        TerminateProcess(hProcess, 0);
        CloseHandle(hProcess);
    };

    CloseHandle(seiw.hProcess);
    return 0;
}
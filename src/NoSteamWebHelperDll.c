#include <windows.h>
#include <wtsapi32.h>

typedef LONG (*fnNtSuspendProcess)(HANDLE);
WNDPROC SteamWndProc;
HANDLE hProcess;

LRESULT NoBrowserWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    if (Msg == WM_DESTROY)
    {
        TerminateProcess(hProcess, 0);
        CloseHandle(hProcess);
    };
    return CallWindowProcW(SteamWndProc, hWnd, Msg, wParam, lParam);
}

void WinEventProc(
    HWINEVENTHOOK hWinEventHook,
    DWORD event,
    HWND hWnd,
    LONG idObject,
    LONG idChild,
    DWORD idEventThread,
    DWORD dwmsEventTime)
{
    if (event != EVENT_OBJECT_HIDE &&
        idObject != OBJID_WINDOW &&
        idChild != CHILDID_SELF)
        return;
    UnhookWinEvent(hWinEventHook);
    SteamWndProc = (WNDPROC)GetWindowLongPtrW(hWnd, GWL_WNDPROC);
    SetWindowLongPtrW(hWnd, GWL_WNDPROC, (LONG_PTR)NoBrowserWndProc);
    PostQuitMessage(0);
}

DWORD MainThread()
{
    MSG msg;
    WTS_PROCESS_INFOW *pWPI;
    DWORD dwCount;
    HANDLE hJob = CreateJobObjectW(0, L"Steam");
    HMODULE hLibModule = LoadLibraryW(L"ntdll.dll");
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = {.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE};
    fnNtSuspendProcess NtSuspendProcess = (fnNtSuspendProcess)GetProcAddress(hLibModule, "NtSuspendProcess");
    FreeLibrary(hLibModule);
    SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli));
    // AssignProcessToJobObject(hJob, GetCurrentProcess());
    SetWinEventHook(EVENT_OBJECT_HIDE, EVENT_OBJECT_HIDE, 0, (WINEVENTPROC)WinEventProc, GetCurrentProcessId(), 0, WINEVENT_OUTOFCONTEXT);

    while (GetMessageW(&msg, 0, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    };

    if (!WTSEnumerateProcessesW(WTS_CURRENT_SERVER, 0, 1, &pWPI, &dwCount))
        return 1;
    for (DWORD i = 0; i < dwCount; i++)
    {
        wcslwr(pWPI[i].pProcessName);
        if (wcscmp(L"steamwebhelper.exe", pWPI[i].pProcessName))
            continue;
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pWPI[i].ProcessId);
        NtSuspendProcess(hProcess);
        AssignProcessToJobObject(hJob, hProcess);
        SetProcessWorkingSetSize(hProcess, -1, -1);
        WTSFreeMemory(pWPI);
        break;
    };
    return TRUE;
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, 0, 0, 0);
    return TRUE;
}
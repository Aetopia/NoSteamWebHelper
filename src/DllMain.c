#include <windows.h>
#include <wtsapi32.h>
#include <winternl.h>

DWORD WINAPI ThreadProc(LPVOID);

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static NOTIFYICONDATAW _ = {.cbSize = sizeof(NOTIFYICONDATAW),
                                .uCallbackMessage = WM_USER,
                                .uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP,
                                .szTip = L"NoSteamWebHelper"};
    static UINT $ = WM_NULL;

    switch (uMsg)
    {
    case WM_CREATE:
        $ = RegisterWindowMessageW(L"TaskbarCreated");
        _.hWnd = hWnd;
        _.hIcon = LoadIconW(NULL, IDI_APPLICATION);
        Shell_NotifyIconW(NIM_ADD, &_);
        break;

    case WM_USER:
        if (lParam == WM_RBUTTONDOWN)
        {
            POINT _ = {};
            HMENU hMenu = CreatePopupMenu();
            GetCursorPos(&_);
            AppendMenuW(hMenu, MF_STRING, FALSE, L"On");
            AppendMenuW(hMenu, MF_STRING, TRUE, L"Off");
            SetForegroundWindow(hWnd);
            RegSetKeyValueW(
                HKEY_CURRENT_USER, L"SOFTWARE\\Valve\\Steam", L"RunningAppID", REG_DWORD,
                &((DWORD){TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, _.x, _.y,
                                         0, hWnd, NULL)
                              ? TRUE
                              : FALSE}),
                sizeof(DWORD));
            DestroyMenu(hMenu);
        }
        break;

    default:
        if (uMsg == $)
            Shell_NotifyIconW(NIM_ADD, &_);
        break;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

VOID CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild,
                           DWORD dwEventThread, DWORD dwmsEventTime)
{
    WCHAR szClassName[16] = {};
    GetClassNameW(hwnd, szClassName, 16);

    if (CompareStringOrdinal(L"vguiPopupWindow", -1, szClassName, -1, FALSE) != CSTR_EQUAL ||
        GetWindowTextLengthW(hwnd) < 1)
        return;

    CloseHandle(CreateThread(NULL, 0, ThreadProc, (LPVOID)TRUE, 0, NULL));

    HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, dwEventThread);
    HKEY hKey = NULL;
    RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Valve\\Steam", REG_OPTION_NON_VOLATILE, KEY_NOTIFY | KEY_QUERY_VALUE,
                  &hKey);

    while (!RegNotifyChangeKeyValue(hKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET, NULL, FALSE))
    {
        DWORD _ = 0;
        RegGetValueW(hKey, NULL, L"RunningAppID", RRF_RT_REG_DWORD, NULL, (PVOID)&_, &((DWORD){sizeof(DWORD)}));
        (_ ? SuspendThread : ResumeThread)(hThread);
        if (_)
        {
            WTS_PROCESS_INFOW *pProcessInfo = {};
            DWORD $ = {};

            WTSEnumerateProcessesW(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pProcessInfo, &$);
            for (DWORD _ = {}; _ < $; _++)
                if (CompareStringOrdinal(pProcessInfo[_].pProcessName, -1, L"steamwebhelper.exe", -1, TRUE) ==
                    CSTR_EQUAL)
                {
                    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_TERMINATE, FALSE,
                                                  pProcessInfo[_].ProcessId);

                    PROCESS_BASIC_INFORMATION _ = {};
                    NtQueryInformationProcess(hProcess, ProcessBasicInformation, &_, sizeof(PROCESS_BASIC_INFORMATION),
                                              NULL);

                    if (_.InheritedFromUniqueProcessId == GetCurrentProcessId())
                        TerminateProcess(hProcess, EXIT_SUCCESS);

                    CloseHandle(hProcess);
                }
            WTSFreeMemory(pProcessInfo);
        }
    }
}

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
    MSG _ = {};

    if (lpParameter)
    {
        HINSTANCE hInstance = GetModuleHandleW(NULL);
        RegisterClassW(&((WNDCLASSW){.lpszClassName = L" ", .hInstance = hInstance, .lpfnWndProc = (WNDPROC)WndProc}));
        CreateWindowExW(WS_EX_LEFT | WS_EX_LTRREADING, L" ", NULL, WS_OVERLAPPED, 0, 0, 0, 0, NULL, NULL, hInstance,
                        NULL);
    }
    else
        SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_CREATE, NULL, WinEventProc, GetCurrentProcessId(), 0,
                        WINEVENT_OUTOFCONTEXT);

    while (GetMessageW(&_, NULL, 0, 0))
    {
        TranslateMessage(&_);
        DispatchMessageW(&_);
    }
    return EXIT_SUCCESS;
}

BOOL WINAPI DllMainCRTStartup(HINSTANCE hLibModule, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hLibModule);
        CloseHandle(CreateThread(NULL, 0, ThreadProc, (LPVOID)FALSE, 0, NULL));
    }
    return TRUE;
}
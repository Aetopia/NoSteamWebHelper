#include <windows.h>
#include <wtsapi32.h>
#include <winternl.h>

static DWORD WINAPI ThreadProc(LPVOID lpParameter);

static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static NOTIFYICONDATAW _ = {.cbSize = sizeof(NOTIFYICONDATAW),
                                .uCallbackMessage = WM_USER,
                                .uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP,
                                .szTip = L"Steam WebHelper"};
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
            HMENU hMenu = CreatePopupMenu();
            AppendMenuW(hMenu, MF_STRING, FALSE, L"On");
            AppendMenuW(hMenu, MF_STRING, TRUE, L"Off");
            SetForegroundWindow(hWnd);

            POINT _ = {};
            GetCursorPos(&_);
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

static VOID CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild,
                                  DWORD dwEventThread, DWORD dwmsEventTime)
{
    WCHAR szClassName[16] = {};
    GetClassNameW(hwnd, szClassName, 16);

    if (CompareStringOrdinal(L"vguiPopupWindow", -1, szClassName, -1, FALSE) != CSTR_EQUAL ||
        GetWindowTextLengthW(hwnd) < 1)
        return;

    CloseHandle(CreateThread(NULL, 0, ThreadProc, (LPVOID)FALSE, 0, NULL));

    HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, dwEventThread);
    HKEY hKey = NULL;
    RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Valve\\Steam", REG_OPTION_NON_VOLATILE, KEY_NOTIFY | KEY_QUERY_VALUE,
                  &hKey);

    while (!RegNotifyChangeKeyValue(hKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET, NULL, FALSE))
    {
        BOOL _ = FALSE;
        RegGetValueW(hKey, NULL, L"RunningAppID", RRF_RT_REG_DWORD, NULL, (PVOID)&_, &((DWORD){sizeof(BOOL)}));
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

static DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
    if (lpParameter)
        SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_CREATE, NULL, WinEventProc, GetCurrentProcessId(), (DWORD){},
                        WINEVENT_OUTOFCONTEXT);
    else
        CreateWindowExW(
            WS_EX_LEFT | WS_EX_LTRREADING,
            (LPCWSTR)(ULONG_PTR)RegisterClassW(&((WNDCLASSW){
                .lpszClassName = L" ", .hInstance = GetModuleHandleW(NULL), .lpfnWndProc = (WNDPROC)WndProc})),
            NULL, WS_OVERLAPPED, 0, 0, 0, 0, NULL, NULL, GetModuleHandleW(NULL), NULL);

    MSG _ = {};
    while (GetMessageW(&_, NULL, (UINT){}, (UINT){}))
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
        CloseHandle(CreateThread(NULL, 0, ThreadProc, (LPVOID)TRUE, 0, NULL));
    }
    return TRUE;
}
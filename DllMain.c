#include <windows.h>

DWORD ThreadProc(LPVOID);

LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static NOTIFYICONDATAW s_Data = {.cbSize = sizeof(NOTIFYICONDATAW),
                                     .uCallbackMessage = WM_USER,
                                     .uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP,
                                     .szTip = L"NoSteamWebHelper"};
    static UINT s_uTaskbarRestart = WM_NULL;

    switch (uMsg)
    {
    case WM_CREATE:
        s_uTaskbarRestart = RegisterWindowMessageW(L"TaskbarCreated");
        s_Data.hWnd = hWnd;
        s_Data.hIcon = LoadIconW(NULL, IDI_APPLICATION);
        Shell_NotifyIconW(NIM_ADD, &s_Data);
        break;

    case WM_USER:
        if (lParam == WM_RBUTTONDOWN)
        {
            POINT Point = {};
            HMENU hMenu = CreatePopupMenu();
            GetCursorPos(&Point);
            AppendMenuW(hMenu, MF_STRING, 1, L"On");
            AppendMenuW(hMenu, MF_STRING, 2, L"Off");
            SetForegroundWindow(hWnd);
            RegSetKeyValueW(
                HKEY_CURRENT_USER, L"SOFTWARE\\Valve\\Steam", L"RunningAppID", REG_DWORD,
                &((DWORD){TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, Point.x,
                                         Point.y, 0, hWnd, NULL) == 2
                              ? 1
                              : 0}),
                sizeof(DWORD));
            DestroyMenu(hMenu);
        }
        break;

    default:
        if (uMsg == s_uTaskbarRestart)
            Shell_NotifyIconW(NIM_ADD, &s_Data);
        break;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

VOID CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild,
                           DWORD dwEventThread, DWORD dwmsEventTime)
{
    WCHAR szClassName[17] = {}, szString[10] = {};
    GetClassNameW(hwnd, szClassName, 17);
    GetWindowTextW(hwnd, szString, 10);

    if (CompareStringOrdinal(L"vguiPopupWindow", -1, szClassName, -1, FALSE) != CSTR_EQUAL ||
        CompareStringOrdinal(L"Untitled", -1, szString, -1, FALSE) != CSTR_EQUAL)
        return;

    CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, (LPVOID)TRUE, 0, NULL));

    HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, dwEventThread);
    HKEY hKey = NULL;
    RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Valve\\Steam", REG_OPTION_NON_VOLATILE, KEY_NOTIFY | KEY_QUERY_VALUE,
                  &hKey);

    while (!RegNotifyChangeKeyValue(hKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET, NULL, FALSE))
    {
        DWORD dwRunningAppID = 0;
        RegGetValueW(hKey, NULL, L"RunningAppID", RRF_RT_REG_DWORD, NULL, (PVOID)&dwRunningAppID,
                     &((DWORD){sizeof(DWORD)}));

        (dwRunningAppID ? SuspendThread : ResumeThread)(hThread);
        if (dwRunningAppID)
            EndTask(FindWindowW(L"SDL_app", L"Steam"), FALSE, TRUE);
    }
}

DWORD ThreadProc(LPVOID lpParameter)
{
    MSG msg = {};

    if (lpParameter)
    {
        HINSTANCE hInstance = GetModuleHandleW(NULL);
        RegisterClassW(&((WNDCLASSW){
            .lpszClassName = L"NoSteamWebHelper", .hInstance = hInstance, .lpfnWndProc = (WNDPROC)WndProc}));
        CreateWindowExW(WS_EX_LEFT | WS_EX_LTRREADING, L"NoSteamWebHelper", NULL, WS_OVERLAPPED, 0, 0, 0, 0, NULL, NULL,
                        hInstance, NULL);
    }
    else
        SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_CREATE, NULL, WinEventProc, GetCurrentProcessId(), 0,
                        WINEVENT_OUTOFCONTEXT);

    while (GetMessageW(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}

BOOL WINAPI DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hinstDLL);
        CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, (LPVOID)FALSE, 0, NULL));
    }
    return TRUE;
}
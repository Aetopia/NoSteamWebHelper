#include <windows.h>

VOID CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild,
                           DWORD dwEventThread, DWORD dwmsEventTime)
{
    WCHAR szClassName[17] = {}, szString[10] = {};
    GetClassNameW(hwnd, szClassName, 17);
    GetWindowTextW(hwnd, szString, 10);

    if (CompareStringOrdinal(L"vguiPopupWindow", -1, szClassName, -1, FALSE) != CSTR_EQUAL ||
        CompareStringOrdinal(L"Untitled", -1, szString, -1, FALSE) != CSTR_EQUAL)
        return;

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

DWORD ThreadProc()
{
    MSG msg = {};
    SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_CREATE, NULL, WinEventProc, GetCurrentProcessId(), 0,
                    WINEVENT_OUTOFCONTEXT);
    while (GetMessageW(&msg, NULL, 0, 0))
        ;
    return 0;
}

BOOL WINAPI DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hinstDLL);
        CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, NULL, 0, NULL));
    }
    return TRUE;
}
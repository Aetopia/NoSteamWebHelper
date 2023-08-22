#include <windows.h>
#include <tlhelp32.h>

DWORD ThreadProc(LPVOID lpParameter)
{
    HKEY hKey = NULL,
         hSubKey = NULL;
    HANDLE hThread = NULL,
           hSnapshot = NULL,
           hProcess = NULL,
           hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    PROCESSENTRY32W pe = {.dwSize = sizeof(PROCESSENTRY32W)};
    WCHAR rgwcName[MAX_PATH] = {0},
          rgwcData[MAX_PATH] = {0};
    BOOL bRunning = FALSE;
    DWORD dwIndex = 0, cbData = 0;

    RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Valve\\Steam\\Apps", 0, KEY_ALL_ACCESS, &hKey);
    do
    {
        RegNotifyChangeKeyValue(hKey, TRUE, REG_NOTIFY_CHANGE_LAST_SET, hEvent, TRUE);
        WaitForSingleObject(hEvent, INFINITE);
        while (!RegEnumKeyExW(hKey, dwIndex, rgwcName, &((DWORD){255}), NULL, NULL, NULL, NULL))
        {
            RegOpenKeyExW(hKey, rgwcName, 0, KEY_ALL_ACCESS, &hSubKey);
            RegGetValueW(hSubKey, NULL, L"Name", RRF_RT_REG_SZ, NULL, NULL, &cbData);
            if (cbData)
            {
                LPWSTR lpData = (LPWSTR)_malloca(cbData);
                RegGetValueW(hSubKey, NULL, L"Name", RRF_RT_REG_SZ, NULL, lpData, &cbData);
                RegGetValueW(hSubKey, NULL, L"Running", RRF_RT_REG_DWORD, NULL, &bRunning, &((DWORD){sizeof(BOOL)}));
                if (bRunning && !wcslen(rgwcData))
                {
                    wcscpy(rgwcData, lpData);
                    SuspendThread((hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE,
                                                        GetWindowThreadProcessId(
                                                            FindWindowW(L"vguiPopupWindow", L"Untitled"),
                                                            NULL))));
                    CloseHandle(hThread);
                    if (Process32FirstW((hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)), &pe))
                        do
                        {
                            if (!wcscmp(L"steamwebhelper.exe", _wcslwr(pe.szExeFile)))
                            {
                                TerminateProcess((hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID)), 0);
                                CloseHandle(hThread);
                            }
                        } while (Process32NextW(hSnapshot, &pe));
                    CloseHandle(hSnapshot);
                }
                else if (!bRunning && !wcscmp(lpData, rgwcData))
                {
                    memset(rgwcData, 0, sizeof(rgwcData));
                    ResumeThread((hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE,
                                                       GetWindowThreadProcessId(
                                                           FindWindowW(L"vguiPopupWindow", L"Untitled"),
                                                           NULL))));
                    CloseHandle(hThread);
                }
            }
            RegCloseKey(hSubKey);
            dwIndex += 1;
        }
        dwIndex = 0;
    } while (TRUE);

    return EXIT_SUCCESS;
}

BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, NULL, 0, NULL);
    }
    return TRUE;
}
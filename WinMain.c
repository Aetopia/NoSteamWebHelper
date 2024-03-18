#include <windows.h>

int WinMainCRTStartup()
{
    DWORD dwSize = MAX_PATH;
    LPWSTR lpExeName = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSize * sizeof(WCHAR));

    while (!QueryFullProcessImageNameW(GetCurrentProcess(), 0, lpExeName, &dwSize))
        lpExeName = HeapReAlloc(lpExeName, HEAP_ZERO_MEMORY, lpExeName, (dwSize += MAX_PATH) * sizeof(WCHAR));

    for (DWORD nChar = lstrlenW(lpExeName); nChar < -1; nChar--)
        if (lpExeName[nChar] == '\\')
        {
            lpExeName[nChar] = '\0';
            SetCurrentDirectoryW(lpExeName);
            break;
        }

    PROCESS_INFORMATION ProcessInformation = {};
    if (CreateProcessW(NULL,
                       lstrcpyW(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 122),
                                L"steam.exe -silent -cef-single-process -cef-disable-breakpad"),
                       NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &((STARTUPINFOW){}), &ProcessInformation))
    {
        HKEY hKey = NULL;
        RegCreateKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Valve\\Steam", 0, NULL, REG_OPTION_NON_VOLATILE,
                        KEY_CREATE_SUB_KEY | KEY_SET_VALUE, NULL, &hKey, NULL);
        RegSetValueExW(hKey, L"GPUAccelWebViewsV3", 0, REG_DWORD, (BYTE *)&((DWORD){}), sizeof(DWORD));
        RegCloseKey(hKey);
    }
    LPVOID lpBaseAddress = VirtualAllocEx(ProcessInformation.hProcess, NULL, dwSize = 42, MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(ProcessInformation.hProcess, lpBaseAddress, L"NoSteamWebHelper.dll", dwSize, NULL);
    CloseHandle(CreateRemoteThread(ProcessInformation.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW,
                                   lpBaseAddress, 0, NULL));
    ResumeThread(ProcessInformation.hThread);
    CloseHandle(ProcessInformation.hThread);
    CloseHandle(ProcessInformation.hProcess);

    ExitProcess(0);
    return 0;
}
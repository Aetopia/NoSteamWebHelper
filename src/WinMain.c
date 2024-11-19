#include <windows.h>

VOID WinMainCRTStartup()
{
    DWORD dwSize = MAX_PATH;
    LPWSTR lpExeName = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSize * sizeof(WCHAR));

    while (!QueryFullProcessImageNameW(GetCurrentProcess(), 0, lpExeName, &dwSize))
        lpExeName = HeapReAlloc(lpExeName, HEAP_ZERO_MEMORY, lpExeName, (dwSize += MAX_PATH) * sizeof(WCHAR));

    for (DWORD _ = lstrlenW(lpExeName); _ < -1; _--)
        if (lpExeName[_] == '\\')
        {
            lpExeName[_] = '\0';
            SetCurrentDirectoryW(lpExeName);
            break;
        }

    PROCESS_INFORMATION _ = {};
    CreateProcessW(NULL,
                   lstrcatW(lstrcpyW(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                               22 + (sizeof(WCHAR) * lstrlenW(GetCommandLineW()))),
                                     L"steam.exe "),
                            GetCommandLineW()),
                   NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &((STARTUPINFOW){}), &_);

    LPVOID lpBaseAddress = VirtualAllocEx(_.hProcess, NULL, dwSize = 42, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    WriteProcessMemory(_.hProcess, lpBaseAddress, L"NoSteamWebHelper.dll", dwSize, NULL);

    HANDLE hThread =
        CreateRemoteThread(_.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, lpBaseAddress, 0, NULL);
    WaitForSingleObject(hThread, INFINITE);
    VirtualFreeEx(_.hProcess, lpBaseAddress, 0, MEM_RELEASE);

    ResumeThread(_.hThread);
    CloseHandle(_.hThread);
    CloseHandle(_.hProcess);

    ExitProcess(EXIT_SUCCESS);
}
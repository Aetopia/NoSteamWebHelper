#include <windows.h>

int wWinMainCRTStartup()
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
    CreateProcessW(
        NULL,
        lstrcatW(
            lstrcpyW(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 164 + (sizeof(WCHAR) * lstrlenW(GetCommandLineW()))),
                     L"steam.exe -cef-single-process -cef-disable-breakpad -cef-disable-gpu-compositing "),
            GetCommandLineW()),
        NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &((STARTUPINFOW){}), &ProcessInformation);

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
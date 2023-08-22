#include <windows.h>
#define printf __builtin_printf

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, INT nShowCmd)
{
    LPVOID lpBaseAddress = NULL;
    SHELLEXECUTEINFOW sei = {.cbSize = sizeof(SHELLEXECUTEINFOW),
                             .lpParameters = lpCmdLine,
                             .lpFile = L"steam.exe",
                             .fMask = SEE_MASK_NOCLOSEPROCESS,
                             .nShow = SW_SHOWNORMAL};
    DWORD cbData = 0, dwSize = sizeof(WCHAR) * (wcslen(L"NoSteamWebHelper.dll") + 1);

  (VOID)  RegGetValueW(HKEY_CURRENT_USER, L"SOFTWARE\\Valve\\Steam", L"SteamPath", RRF_RT_REG_SZ, NULL, NULL, &cbData);
    sei.lpDirectory = (LPCWSTR)_malloca(cbData);
  (VOID)   RegGetValueW(HKEY_CURRENT_USER, L"SOFTWARE\\Valve\\Steam", L"SteamPath", RRF_RT_REG_SZ, NULL, (PVOID)sei.lpDirectory, &cbData);
  (VOID)   SetCurrentDirectoryW(sei.lpDirectory);

  (VOID)   ShellExecuteExW(&sei);
    lpBaseAddress = VirtualAllocEx(sei.hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
 (VOID)    WriteProcessMemory(sei.hProcess, lpBaseAddress, L"NoSteamWebHelper.dll", dwSize, NULL);
 (VOID)    CloseHandle(CreateRemoteThread(sei.hProcess, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, lpBaseAddress, 0, NULL));
  (VOID)   CloseHandle(sei.hProcess);
    return EXIT_SUCCESS;
}
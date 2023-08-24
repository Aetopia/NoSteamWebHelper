#include <windows.h>

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, INT nShowCmd)
{
  LPVOID lpBaseAddress = NULL;
  SHELLEXECUTEINFOW sei = {.cbSize = sizeof(SHELLEXECUTEINFOW),
                           .lpParameters = lpCmdLine,
                           .lpFile = L"steam.exe",
                           .fMask = SEE_MASK_NOCLOSEPROCESS,
                           .nShow = SW_SHOWNORMAL};
  DWORD cbData = 0,
        dwSize = sizeof(WCHAR) * (wcslen(L"NoSteamWebHelper.dll") + 1);

  sei.lpParameters = (LPCWSTR)_malloca(sizeof(WCHAR) *
                                       (wcslen(lpCmdLine) +
                                        wcslen(L" -oldtraymenu -cef-single-process -cef-disable-breakpad") +
                                        1));
  (VOID) wcscpy((LPWSTR)sei.lpParameters, lpCmdLine);
  (VOID) wcscat((LPWSTR)sei.lpParameters, L" -oldtraymenu -cef-single-process -cef-disable-breakpad");

  (VOID) RegGetValueW(HKEY_CURRENT_USER, L"SOFTWARE\\Valve\\Steam", L"SteamPath", RRF_RT_REG_SZ, NULL, NULL, &cbData);
  sei.lpDirectory = (LPCWSTR)_malloca(cbData);
  (VOID) RegGetValueW(HKEY_CURRENT_USER, L"SOFTWARE\\Valve\\Steam", L"SteamPath", RRF_RT_REG_SZ, NULL, (PVOID)sei.lpDirectory, &cbData);
  (VOID) SetCurrentDirectoryW(sei.lpDirectory);

  (VOID) ShellExecuteExW(&sei);
  lpBaseAddress = VirtualAllocEx(sei.hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
  (VOID) WriteProcessMemory(sei.hProcess, lpBaseAddress, L"NoSteamWebHelper.dll", dwSize, NULL);
  (VOID) CloseHandle(CreateRemoteThread(sei.hProcess, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, lpBaseAddress, 0, NULL));
  (VOID) CloseHandle(sei.hProcess);

  return EXIT_SUCCESS;
}
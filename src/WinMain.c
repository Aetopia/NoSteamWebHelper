#define  _WIN32_WINNT _WIN32_WINNT_WIN10
#include <windows.h>

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, INT nShowCmd)
{
  LPVOID lpBaseAddress = NULL;
  HANDLE hProcess = GetCurrentProcess();
  WCHAR *lpExeName = NULL;
  DWORD cbData = 0,
        dwSize = sizeof(WCHAR) * (wcslen(L"NoSteamWebHelper.dll") + 1);
  SHELLEXECUTEINFOW sei = {.cbSize = sizeof(SHELLEXECUTEINFOW),
                           .lpParameters = lpCmdLine,
                           .lpFile = L"steam.exe",
                           .fMask = SEE_MASK_NOCLOSEPROCESS,
                           .nShow = SW_SHOWNORMAL};

  sei.lpParameters = (LPCWSTR)_malloca(sizeof(WCHAR) *
                                       (wcslen(lpCmdLine) +
                                        wcslen(L" -oldtraymenu -cef-single-process -cef-disable-breakpad") +
                                        1));
  (VOID) wcscpy((LPWSTR)sei.lpParameters, lpCmdLine);
  (VOID) wcscat((LPWSTR)sei.lpParameters, L" -oldtraymenu -cef-single-process -cef-disable-breakpad");
  dwSize = 0;

  do
    lpExeName = realloc(lpExeName, sizeof(WCHAR) * (dwSize += 1));
  while (!QueryFullProcessImageNameW(hProcess, 0, lpExeName, &dwSize));
  for (DWORD dwIndex = dwSize; dwIndex < -1; dwIndex -= 1)
  {
    if (lpExeName[dwIndex] == '\\')
    {
      lpExeName[dwIndex] = '\0';
      (VOID)CloseHandle(hProcess);
      (VOID) SetCurrentDirectoryW(lpExeName);
      free(lpExeName);
      break;
    }
  }

  (VOID) ShellExecuteExW(&sei);
  lpBaseAddress = VirtualAllocEx(sei.hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
  (VOID) WriteProcessMemory(sei.hProcess, lpBaseAddress, L"NoSteamWebHelper.dll", dwSize, NULL);
  (VOID) CloseHandle(CreateRemoteThread(sei.hProcess, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, lpBaseAddress, 0, NULL));
  (VOID) CloseHandle(sei.hProcess);

  return EXIT_SUCCESS;
}
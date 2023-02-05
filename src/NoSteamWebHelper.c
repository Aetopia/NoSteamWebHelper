#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <winternl.h>
#include <dbghelp.h>
#include <wtsapi32.h>
#include <libgen.h>
#include <shlwapi.h>

DWORD GetProcessByName(char *process)
{
    process = strlwr(process);
    WTS_PROCESS_INFO *pWPI;
    DWORD pid, dwProcessCount;
    if (!WTSEnumerateProcesses(WTS_CURRENT_SERVER,
                               0,
                               1,
                               &pWPI,
                               &dwProcessCount))
        return 0;

    for (int i = 0; i < dwProcessCount; i++)
    {
        if (strcmp(
                strlwr(pWPI[i].pProcessName),
                process))
            continue;
        pid = pWPI[i].ProcessId;
        break;
    };
    WTSFreeMemory(pWPI);
    return pid;
}

void WebHelper(BOOL bEnable)
{
    SYMBOL_INFO *symbol;
    THREADENTRY32 te = {.dwSize = sizeof(THREADENTRY32)};
    ULONG64 dwStartAddress;
    DWORD pid = GetProcessByName("steam.exe");
    HANDLE hThread,
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid),
        hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

    // Due to permissions, the program might not be able to get a process handle to Steam.
    if (!hProcess)
    {
        MessageBox(0,
                   "Couldn't obtain a Process Handle to Steam.",
                   "Error",
                   MB_ICONERROR | MB_SYSTEMMODAL);
        ExitProcess(1);
    };

    // Intiliaze a Symbol Handler for Steam.
    SymInitialize(hProcess, NULL, TRUE);
    if (Thread32First(hThreadSnapshot, &te))
    {
        do
        {
            if (te.th32OwnerProcessID != pid)
                continue;

            hThread = OpenThread(THREAD_QUERY_INFORMATION | THREAD_SUSPEND_RESUME,
                                 FALSE,
                                 te.th32ThreadID);
            NtQueryInformationThread(hThread,
                                     ThreadQuerySetWin32StartAddress,
                                     &dwStartAddress,
                                     sizeof(PVOID),
                                     NULL);

            symbol = (SYMBOL_INFO *)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
            symbol->MaxNameLen = 255,
            symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            SymFromAddr(hProcess, (DWORD64)(dwStartAddress), 0, symbol);

            // Check for functions that use the CreateInterface function, this is used to interface with the Steam WebHelper.
            if (!strcmp("CreateInterface", symbol->Name))
                if (bEnable)
                    ResumeThread(hThread);
                else
                    SuspendThread(hThread);

            free(symbol);
            CloseHandle(hThread);
        } while (Thread32Next(hThreadSnapshot, &te));
    };
    CloseHandle(hThreadSnapshot);
    SymCleanup(hProcess);
    CloseHandle(hProcess);

    // Kill any Steam WebHelper processes that are still running.
    while (TRUE && !bEnable)
    {
        pid = GetProcessByName("steamwebhelper.exe");
        if (!pid)
            break;
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        TerminateProcess(hProcess, 0);
        CloseHandle(hProcess);
    };
};

int main(int argc, char *argv[])
{
    if (argc != 3 && argc != 2)
    {
        MessageBox(0,
                   "Usage: NoSteamWebHelper.exe <Program> <Process>\n\nhttps:/github.com/Aetopia/NoSteamWebHelper",
                   "NoSteamWebHelper",
                   MB_SYSTEMMODAL);
        return 1;
    };
    if (PathIsRelative(argv[1]))
    {
        MessageBox(0,
                   "Please use an absolute path for the program.",
                   "Error",
                   MB_ICONERROR | MB_SYSTEMMODAL);
        return 1;
    };

    FILE *file;
    char app[MAX_PATH], *args;
    int size;
    MSG msg;
    HANDLE hProcess, hJob = CreateJobObject(NULL, NULL);
    DWORD pid;
    STARTUPINFO si = {.cb = sizeof(STARTUPINFO)};
    PROCESS_INFORMATION pi;
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = {
        .BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE |
                                            JOB_OBJECT_LIMIT_BREAKAWAY_OK};
    SHELLEXECUTEINFO sei = {
        .cbSize = sizeof(SHELLEXECUTEINFO),
        .lpVerb = "open",
        .fMask = SEE_MASK_NOCLOSEPROCESS,
        .nShow = SW_SHOWNORMAL,
    };
    SetInformationJobObject(hJob,
                            JobObjectExtendedLimitInformation,
                            &jeli,
                            sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION));

    strcpy(app, argv[1]);
    dirname(argv[1]);
    SetCurrentDirectory(argv[1]);
    sei.lpFile = app;
    sei.lpDirectory = argv[1];

    file = fopen("LaunchOptions.txt", "r");
    fseek(file, 0, SEEK_END);
    size = ftell(file) + 1;
    fseek(file, 0, SEEK_SET);
    args = malloc(size);
    fgets(args, size, file);
    fclose(file);
    sei.lpParameters = args;

    if (!ShellExecuteEx(&sei))
    {
        MessageBox(0,
                   "Couldn't execute the program.",
                   "Error",
                   MB_ICONERROR | MB_SYSTEMMODAL);
        return 1;
    };
    hProcess = sei.hProcess;
    free(args);

    do
    {
        if (argc != 3)
            break;
        pid = GetProcessByName(argv[2]);
        if (!pid)
            continue;
        hProcess = OpenProcess(PROCESS_SET_QUOTA |
                                   PROCESS_TERMINATE |
                                   SYNCHRONIZE,
                               FALSE,
                               pid);
        break;
    } while (!SleepEx(1, TRUE));
    AssignProcessToJobObject(hJob, hProcess);
    WebHelper(FALSE);
    WaitForSingleObject(hProcess, INFINITE);
    WebHelper(TRUE);
    return 0;
}

#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

/**
 * Function wich find the process id of the specified process.
 * \param lpProcessName : name of the target process.
 * \return : the process id if the process is found else -1.
 */
DWORD GetProcessByName(const char* lpProcessName)
{
    char lpCurrentProcessName[255];

    PROCESSENTRY32 ProcList {};
    ProcList.dwSize = sizeof(ProcList);

    const HANDLE hProcList = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcList == INVALID_HANDLE_VALUE)
        return -1;

    if (!Process32First(hProcList, &ProcList))
        return -1;

    wcstombs_s(nullptr, lpCurrentProcessName, ProcList.szExeFile, 255);

    if (lstrcmpA(lpCurrentProcessName, lpProcessName) == 0)
        return ProcList.th32ProcessID;

    while (Process32Next(hProcList, &ProcList))
    {
        wcstombs_s(nullptr, lpCurrentProcessName, ProcList.szExeFile, 255);

        if (lstrcmpA(lpCurrentProcessName, lpProcessName) == 0)
            return ProcList.th32ProcessID;
    }

    return -1;
}

int main(const int argc, char* argv[])
{
    char* lpDLLName;
    char* lpProcessName;
    char lpFullDLLPath[MAX_PATH];

    if (argc == 3)
    {
        lpDLLName = argv[1];
        lpProcessName = argv[2];
    }
    else
    {
        printf("[HELP] inject.exe <dll> <process>\n");
        return -1;
    }

    const DWORD dwProcessID = GetProcessByName(lpProcessName);
    if (dwProcessID == (DWORD) -1)
    {
        printf("An error is occured when trying to find the target process.\n");
        return -1;
    }

    printf("[DLL Injector]\n");
    printf("Process : %s\n", lpProcessName);
    printf("Process ID : %i\n\n", (int)dwProcessID);

    const DWORD dwFullPathResult = GetFullPathNameA(lpDLLName, MAX_PATH, lpFullDLLPath, nullptr);
    if (dwFullPathResult == 0)
    {
        printf("An error is occured when trying to get the full path of the DLL.\n");
        return -1;
    }

    const HANDLE hTargetProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessID);
    if (hTargetProcess == INVALID_HANDLE_VALUE)
    {
        printf("An error is occured when trying to open the target process.\n");
        return -1;
    }

    printf("[PROCESS INJECTION]\n");
    printf("Process opened successfully.\n");

    const LPVOID lpPathAddress = VirtualAllocEx(hTargetProcess, nullptr, lstrlenA(lpFullDLLPath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (lpPathAddress == nullptr)
    {
        printf("An error is occured when trying to allocate memory in the target process.\n");
        return -1;
    }

    printf("Memory allocate at 0x%X\n", (UINT)(uintptr_t)lpPathAddress);

    const DWORD dwWriteResult = WriteProcessMemory(hTargetProcess, lpPathAddress, lpFullDLLPath, lstrlenA(lpFullDLLPath) + 1, nullptr);
    if (dwWriteResult == 0)
    {
        printf("An error is occured when trying to write the DLL path in the target process.\n");
        return -1;
    }

    printf("DLL path writen successfully.\n");

    const HMODULE hModule = GetModuleHandleA("kernel32.dll");
    if (hModule == INVALID_HANDLE_VALUE || hModule == nullptr)
        return -1;

    const FARPROC lpFunctionAddress = GetProcAddress(hModule, "LoadLibraryA");
    if (lpFunctionAddress == nullptr)
    {
        printf("An error is occured when trying to get \"LoadLibraryA\" address.\n");
        return -1;
    }

    printf("LoadLibraryA address at 0x%X\n", (UINT)(uintptr_t)lpFunctionAddress);

    const HANDLE hThreadCreationResult = CreateRemoteThread(hTargetProcess, nullptr, 0, (LPTHREAD_START_ROUTINE)lpFunctionAddress, lpPathAddress, 0, nullptr);
    if (hThreadCreationResult == INVALID_HANDLE_VALUE)
    {
        printf("An error is occured when trying to create the thread in the target process.\n");
        return -1;
    }

    printf("DLL Injected !\n");

    return 0;
}
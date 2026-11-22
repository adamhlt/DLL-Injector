#include <Windows.h>
#include <TlHelp32.h>

#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <string>

namespace
{
    /**
     * RAII wrapper that closes a Win32 handle when it goes out of scope.
     */
    class UniqueHandle
    {
    public:
        explicit UniqueHandle(const HANDLE handle = nullptr) : m_handle(handle) {}

        ~UniqueHandle()
        {
            if (m_handle != nullptr && m_handle != INVALID_HANDLE_VALUE)
                CloseHandle(m_handle);
        }

        UniqueHandle(const UniqueHandle&) = delete;
        UniqueHandle& operator=(const UniqueHandle&) = delete;

        HANDLE get() const { return m_handle; }
        bool isValid() const { return m_handle != nullptr && m_handle != INVALID_HANDLE_VALUE; }

    private:
        HANDLE m_handle;
    };

    /**
     * Returns a readable description of a Win32 error code.
     * \param errorCode : code returned by GetLastError().
     * \return : the system message, or a generic text when it cannot be resolved.
     */
    std::wstring FormatError(const DWORD errorCode)
    {
        LPWSTR buffer = nullptr;
        const DWORD length = FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<LPWSTR>(&buffer), 0, nullptr);

        if (length == 0 || buffer == nullptr)
            return L"unknown error (" + std::to_wstring(errorCode) + L")";

        std::wstring message(buffer, length);
        LocalFree(buffer);

        while (!message.empty() && (message.back() == L'\r' || message.back() == L'\n' || message.back() == L' '))
            message.pop_back();

        return message;
    }

    /**
     * Prints an error message and returns the standard failure code.
     */
    int Fail(const wchar_t* stage, const DWORD errorCode)
    {
        std::fwprintf(stderr, L"[ERROR] %ls failed: %ls\n", stage, FormatError(errorCode).c_str());
        return EXIT_FAILURE;
    }

    /**
     * Finds the process id of the specified process.
     * \param lpProcessName : name of the target process (case-insensitive).
     * \return : the process id if the process is found, else 0.
     */
    DWORD GetProcessIdByName(const wchar_t* lpProcessName)
    {
        UniqueHandle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
        if (!snapshot.isValid())
            return 0;

        PROCESSENTRY32W procEntry{};
        procEntry.dwSize = sizeof(procEntry);

        for (BOOL hasEntry = Process32FirstW(snapshot.get(), &procEntry); hasEntry;
             hasEntry = Process32NextW(snapshot.get(), &procEntry))
        {
            if (lstrcmpiW(procEntry.szExeFile, lpProcessName) == 0)
                return procEntry.th32ProcessID;
        }

        return 0;
    }
} // namespace

int wmain(const int argc, wchar_t* argv[])
{
    if (argc != 3)
    {
        std::wprintf(L"Usage: inject.exe <dll> <process>\n");
        return EXIT_FAILURE;
    }

    const wchar_t* lpDLLName = argv[1];
    const wchar_t* lpProcessName = argv[2];

    const DWORD dwProcessID = GetProcessIdByName(lpProcessName);
    if (dwProcessID == 0)
    {
        std::fwprintf(stderr, L"[ERROR] Process not found: %ls\n", lpProcessName);
        return EXIT_FAILURE;
    }

    wchar_t lpFullDLLPath[MAX_PATH];
    const DWORD dwFullPathResult = GetFullPathNameW(lpDLLName, MAX_PATH, lpFullDLLPath, nullptr);
    if (dwFullPathResult == 0 || dwFullPathResult >= MAX_PATH)
        return Fail(L"GetFullPathNameW", GetLastError());

    std::wprintf(L"[DLL Injector]\n");
    std::wprintf(L"Process    : %ls\n", lpProcessName);
    std::wprintf(L"Process ID : %lu\n", static_cast<unsigned long>(dwProcessID));
    std::wprintf(L"DLL        : %ls\n\n", lpFullDLLPath);

    // Request only the rights required for the injection.
    constexpr DWORD dwAccess = PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION |
                               PROCESS_VM_WRITE | PROCESS_VM_READ;

    UniqueHandle hTargetProcess(OpenProcess(dwAccess, FALSE, dwProcessID));
    if (!hTargetProcess.isValid())
        return Fail(L"OpenProcess", GetLastError());

    const SIZE_T dwPathSize = (std::wcslen(lpFullDLLPath) + 1) * sizeof(wchar_t);
    const LPVOID lpPathAddress = VirtualAllocEx(hTargetProcess.get(), nullptr, dwPathSize,
                                                MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (lpPathAddress == nullptr)
        return Fail(L"VirtualAllocEx", GetLastError());

    if (WriteProcessMemory(hTargetProcess.get(), lpPathAddress, lpFullDLLPath, dwPathSize, nullptr) == FALSE)
        return Fail(L"WriteProcessMemory", GetLastError());

    const HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (hKernel32 == nullptr)
        return Fail(L"GetModuleHandleW", GetLastError());

    const auto lpLoadLibrary = reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(hKernel32, "LoadLibraryW"));
    if (lpLoadLibrary == nullptr)
        return Fail(L"GetProcAddress", GetLastError());

    UniqueHandle hThread(CreateRemoteThread(hTargetProcess.get(), nullptr, 0, lpLoadLibrary,
                                            lpPathAddress, 0, nullptr));
    if (!hThread.isValid())
        return Fail(L"CreateRemoteThread", GetLastError());

    std::wprintf(L"DLL injected !\n");

    return EXIT_SUCCESS;
}

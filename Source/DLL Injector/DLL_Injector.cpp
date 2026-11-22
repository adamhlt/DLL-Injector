#include <Windows.h>
#include <TlHelp32.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
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
    std::string FormatError(const DWORD errorCode)
    {
        LPSTR buffer = nullptr;
        const DWORD length = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<LPSTR>(&buffer), 0, nullptr);

        if (length == 0 || buffer == nullptr)
            return "unknown error (" + std::to_string(errorCode) + ")";

        std::string message(buffer, length);
        LocalFree(buffer);

        while (!message.empty() && (message.back() == '\r' || message.back() == '\n' || message.back() == ' '))
            message.pop_back();

        return message;
    }

    /**
     * Prints an error message and returns the standard failure code.
     */
    int Fail(const char* stage, const DWORD errorCode)
    {
        std::fprintf(stderr, "[ERROR] %s failed: %s\n", stage, FormatError(errorCode).c_str());
        return EXIT_FAILURE;
    }

    /**
     * Finds the process id of the specified process.
     * \param lpProcessName : name of the target process (case-insensitive).
     * \return : the process id if the process is found, else 0.
     */
    DWORD GetProcessIdByName(const char* lpProcessName)
    {
        UniqueHandle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
        if (!snapshot.isValid())
            return 0;

        PROCESSENTRY32 procEntry{};
        procEntry.dwSize = sizeof(procEntry);

        for (BOOL hasEntry = Process32First(snapshot.get(), &procEntry); hasEntry;
             hasEntry = Process32Next(snapshot.get(), &procEntry))
        {
            char currentName[255];
            if (wcstombs_s(nullptr, currentName, procEntry.szExeFile, 255) != 0)
                continue;

            if (lstrcmpiA(currentName, lpProcessName) == 0)
                return procEntry.th32ProcessID;
        }

        return 0;
    }
} // namespace

int main(const int argc, char* argv[])
{
    if (argc != 3)
    {
        std::printf("Usage: inject.exe <dll> <process>\n");
        return EXIT_FAILURE;
    }

    const char* lpDLLName = argv[1];
    const char* lpProcessName = argv[2];

    const DWORD dwProcessID = GetProcessIdByName(lpProcessName);
    if (dwProcessID == 0)
    {
        std::fprintf(stderr, "[ERROR] Process not found: %s\n", lpProcessName);
        return EXIT_FAILURE;
    }

    char lpFullDLLPath[MAX_PATH];
    const DWORD dwFullPathResult = GetFullPathNameA(lpDLLName, MAX_PATH, lpFullDLLPath, nullptr);
    if (dwFullPathResult == 0 || dwFullPathResult >= MAX_PATH)
        return Fail("GetFullPathNameA", GetLastError());

    std::printf("[DLL Injector]\n");
    std::printf("Process    : %s\n", lpProcessName);
    std::printf("Process ID : %lu\n", static_cast<unsigned long>(dwProcessID));
    std::printf("DLL        : %s\n\n", lpFullDLLPath);

    // Request only the rights required for the injection.
    constexpr DWORD dwAccess = PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION |
                               PROCESS_VM_WRITE | PROCESS_VM_READ;

    UniqueHandle hTargetProcess(OpenProcess(dwAccess, FALSE, dwProcessID));
    if (!hTargetProcess.isValid())
        return Fail("OpenProcess", GetLastError());

    const SIZE_T dwPathSize = std::strlen(lpFullDLLPath) + 1;
    const LPVOID lpPathAddress = VirtualAllocEx(hTargetProcess.get(), nullptr, dwPathSize,
                                                MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (lpPathAddress == nullptr)
        return Fail("VirtualAllocEx", GetLastError());

    if (WriteProcessMemory(hTargetProcess.get(), lpPathAddress, lpFullDLLPath, dwPathSize, nullptr) == FALSE)
        return Fail("WriteProcessMemory", GetLastError());

    const HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    if (hKernel32 == nullptr)
        return Fail("GetModuleHandleA", GetLastError());

    const auto lpLoadLibrary = reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(hKernel32, "LoadLibraryA"));
    if (lpLoadLibrary == nullptr)
        return Fail("GetProcAddress", GetLastError());

    UniqueHandle hThread(CreateRemoteThread(hTargetProcess.get(), nullptr, 0, lpLoadLibrary,
                                            lpPathAddress, 0, nullptr));
    if (!hThread.isValid())
        return Fail("CreateRemoteThread", GetLastError());

    std::printf("DLL injected !\n");

    return EXIT_SUCCESS;
}

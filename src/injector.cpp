#include "injector/injector.hpp"

#include "injector/error.hpp"
#include "unique_handle.hpp"

#include <string>

namespace injector
{
    namespace
    {
        /**
         * Builds a failure result for the given stage and error code.
         */
        InjectionResult Failure(const wchar_t* stage, const DWORD errorCode)
        {
            return {false, nullptr, std::wstring(stage) + L" failed: " + FormatError(errorCode)};
        }

        /**
         * Builds a failure result for a custom message.
         */
        InjectionResult Failure(const std::wstring& message)
        {
            return {false, nullptr, message};
        }

        /**
         * Frees a block of memory allocated in a remote process.
         */
        class RemoteMemory
        {
        public:
            RemoteMemory(const HANDLE process, const LPVOID address) : m_process(process), m_address(address) {}

            ~RemoteMemory()
            {
                if (m_address != nullptr)
                    VirtualFreeEx(m_process, m_address, 0, MEM_RELEASE);
            }

            RemoteMemory(const RemoteMemory&) = delete;
            RemoteMemory& operator=(const RemoteMemory&) = delete;

            /**
             * Gives up ownership without freeing the buffer, for example when the
             * remote thread may still be reading it.
             */
            void release() { m_address = nullptr; }

            LPVOID get() const { return m_address; }

        private:
            HANDLE m_process;
            LPVOID m_address;
        };

#if defined(_WIN64)
        constexpr bool kInjectorIs64Bit = true;
#else
        constexpr bool kInjectorIs64Bit = false;
#endif

        /**
         * True when the current process is a 32-bit binary running under WOW64.
         */
        bool IsCurrentProcessWow64()
        {
            BOOL isWow64 = FALSE;
            return IsWow64Process(GetCurrentProcess(), &isWow64) != FALSE && isWow64 != FALSE;
        }

        /**
         * Best-effort detection of the target process architecture.
         */
        bool IsTarget64Bit(const HANDLE process)
        {
            const bool osIs64Bit = kInjectorIs64Bit || IsCurrentProcessWow64();
            if (!osIs64Bit)
                return false; // A 32-bit OS only runs 32-bit processes.

            BOOL isWow64 = FALSE;
            if (IsWow64Process(process, &isWow64) == FALSE)
                return kInjectorIs64Bit; // Assume a native process.

            return isWow64 == FALSE;
        }
    } // namespace

    InjectionResult InjectDll(const DWORD processId, const std::wstring& dllPath,
                              const std::chrono::milliseconds timeout)
    {
        // Request only the rights required for the injection.
        constexpr DWORD dwAccess = PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
                                   PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ;

        UniqueHandle hTargetProcess(OpenProcess(dwAccess, FALSE, processId));
        if (!hTargetProcess.isValid())
            return Failure(L"OpenProcess", GetLastError());

        if (IsTarget64Bit(hTargetProcess.get()) != kInjectorIs64Bit)
            return Failure(L"architecture mismatch: the injector and the target process must have the same "
                           L"bitness (build the injector for the target architecture)");

        const SIZE_T dwPathSize = (dllPath.size() + 1) * sizeof(wchar_t);
        const LPVOID lpPathAddress = VirtualAllocEx(hTargetProcess.get(), nullptr, dwPathSize,
                                                    MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (lpPathAddress == nullptr)
            return Failure(L"VirtualAllocEx", GetLastError());

        RemoteMemory remotePath(hTargetProcess.get(), lpPathAddress);

        if (WriteProcessMemory(hTargetProcess.get(), remotePath.get(), dllPath.c_str(), dwPathSize, nullptr) == FALSE)
            return Failure(L"WriteProcessMemory", GetLastError());

        const HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
        if (hKernel32 == nullptr)
            return Failure(L"GetModuleHandleW", GetLastError());

        const auto lpLoadLibrary =
            reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(hKernel32, "LoadLibraryW"));
        if (lpLoadLibrary == nullptr)
            return Failure(L"GetProcAddress", GetLastError());

        UniqueHandle hThread(CreateRemoteThread(hTargetProcess.get(), nullptr, 0, lpLoadLibrary,
                                                remotePath.get(), 0, nullptr));
        if (!hThread.isValid())
            return Failure(L"CreateRemoteThread", GetLastError());

        const DWORD dwWaitResult = WaitForSingleObject(hThread.get(), static_cast<DWORD>(timeout.count()));
        if (dwWaitResult == WAIT_TIMEOUT)
        {
            // The remote thread is still running and may still read the buffer:
            // leak it rather than risk a use-after-free in the target process.
            remotePath.release();
            return Failure(L"timed out while waiting for LoadLibraryW to return");
        }

        if (dwWaitResult != WAIT_OBJECT_0)
            return Failure(L"WaitForSingleObject", GetLastError());

        DWORD dwExitCode = 0;
        if (GetExitCodeThread(hThread.get(), &dwExitCode) == FALSE)
            return Failure(L"GetExitCodeThread", GetLastError());

        // On 64-bit builds the module handle is truncated to its low 32 bits by
        // the thread exit code; it is still a reliable non-null success marker.
        const HMODULE hModule = reinterpret_cast<HMODULE>(static_cast<uintptr_t>(dwExitCode));
        if (hModule == nullptr)
            return Failure(L"LoadLibraryW returned NULL in the target process (the DLL could not be loaded)");

        return {true, hModule, L"DLL injected."};
    }
} // namespace injector

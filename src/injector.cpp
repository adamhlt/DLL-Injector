#include "injector/injector.hpp"

#include "injector/error.hpp"
#include "unique_handle.hpp"

namespace injector
{
    namespace
    {
        /**
         * Builds a failure result for the given stage and error code.
         */
        InjectionResult Failure(const wchar_t* stage, const DWORD errorCode)
        {
            return {false, std::wstring(stage) + L" failed: " + FormatError(errorCode)};
        }
    } // namespace

    InjectionResult InjectDll(const DWORD processId, const std::wstring& dllPath)
    {
        // Request only the rights required for the injection.
        constexpr DWORD dwAccess = PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
                                   PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ;

        UniqueHandle hTargetProcess(OpenProcess(dwAccess, FALSE, processId));
        if (!hTargetProcess.isValid())
            return Failure(L"OpenProcess", GetLastError());

        const SIZE_T dwPathSize = (dllPath.size() + 1) * sizeof(wchar_t);
        const LPVOID lpPathAddress = VirtualAllocEx(hTargetProcess.get(), nullptr, dwPathSize,
                                                    MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (lpPathAddress == nullptr)
            return Failure(L"VirtualAllocEx", GetLastError());

        if (WriteProcessMemory(hTargetProcess.get(), lpPathAddress, dllPath.c_str(), dwPathSize, nullptr) == FALSE)
            return Failure(L"WriteProcessMemory", GetLastError());

        const HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
        if (hKernel32 == nullptr)
            return Failure(L"GetModuleHandleW", GetLastError());

        const auto lpLoadLibrary =
            reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(hKernel32, "LoadLibraryW"));
        if (lpLoadLibrary == nullptr)
            return Failure(L"GetProcAddress", GetLastError());

        // The remote buffer is deliberately not freed here: without waiting for the
        // remote thread it might still be reading the DLL path.
        UniqueHandle hThread(CreateRemoteThread(hTargetProcess.get(), nullptr, 0, lpLoadLibrary,
                                                lpPathAddress, 0, nullptr));
        if (!hThread.isValid())
            return Failure(L"CreateRemoteThread", GetLastError());

        return {true, L"DLL injected."};
    }
} // namespace injector

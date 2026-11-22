#include "cli.hpp"
#include "error.hpp"
#include "injector.hpp"
#include "process.hpp"

#include <Windows.h>

#include <cstdio>
#include <cstdlib>

namespace
{
    /**
     * Process exit codes.
     */
    enum ExitCode
    {
        kSuccess = 0,          ///< The DLL was injected.
        kUsageError = 1,       ///< Invalid command line.
        kProcessNotFound = 2,  ///< No process matched the given name or id.
        kPathError = 3,        ///< The DLL path could not be resolved.
        kInjectionFailed = 4,  ///< The injection itself failed.
    };

    /**
     * Prints an error message and returns the given exit code.
     */
    int Fail(const std::wstring& message, const int exitCode)
    {
        std::fwprintf(stderr, L"[ERROR] %ls\n", message.c_str());
        return exitCode;
    }
} // namespace

int wmain(const int argc, wchar_t* argv[])
{
    std::wstring error;
    const std::optional<injector::cli::Options> options = injector::cli::Parse(argc, argv, error);
    if (!options)
    {
        std::fwprintf(stderr, L"[ERROR] %ls\n\n%ls", error.c_str(), injector::cli::Usage().c_str());
        return kUsageError;
    }

    if (options->help)
    {
        std::fwprintf(stdout, L"%ls", injector::cli::Usage().c_str());
        return kSuccess;
    }

    DWORD dwProcessID = 0;
    if (options->processId)
    {
        dwProcessID = *options->processId;
    }
    else
    {
        const std::optional<DWORD> found = injector::FindProcessId(options->process);
        if (!found)
            return Fail(L"process not found: " + options->process, kProcessNotFound);

        dwProcessID = *found;
    }

    wchar_t lpFullDLLPath[MAX_PATH];
    const DWORD dwFullPathResult = GetFullPathNameW(options->dllPath.c_str(), MAX_PATH, lpFullDLLPath, nullptr);
    if (dwFullPathResult == 0 || dwFullPathResult >= MAX_PATH)
        return Fail(L"could not resolve the DLL path: " + injector::FormatLastError(), kPathError);

    std::wprintf(L"[DLL Injector]\n");
    std::wprintf(L"Process    : %ls\n", options->process.c_str());
    std::wprintf(L"Process ID : %lu\n", static_cast<unsigned long>(dwProcessID));
    std::wprintf(L"DLL        : %ls\n\n", lpFullDLLPath);

    const injector::InjectionResult result = injector::InjectDll(dwProcessID, lpFullDLLPath);
    if (!result.success)
        return Fail(result.message, kInjectionFailed);

    std::wprintf(L"%ls (module: 0x%llX)\n", result.message.c_str(),
                 static_cast<unsigned long long>(reinterpret_cast<uintptr_t>(result.module)));
    return kSuccess;
}

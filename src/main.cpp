#include "cli.hpp"
#include "injector/error.hpp"
#include "injector/injector.hpp"
#include "injector/process.hpp"

#include <Windows.h>

#include <cstdio>
#include <cstdlib>

namespace
{
    /**
     * Prints an error message and returns the standard failure code.
     */
    int Fail(const std::wstring& message)
    {
        std::fwprintf(stderr, L"[ERROR] %ls\n", message.c_str());
        return EXIT_FAILURE;
    }
} // namespace

int wmain(const int argc, wchar_t* argv[])
{
    std::wstring error;
    const std::optional<injector::cli::Options> options = injector::cli::Parse(argc, argv, error);
    if (!options)
    {
        std::fwprintf(stderr, L"[ERROR] %ls\n\n%ls", error.c_str(), injector::cli::Usage().c_str());
        return EXIT_FAILURE;
    }

    if (options->help)
    {
        std::fwprintf(stdout, L"%ls", injector::cli::Usage().c_str());
        return EXIT_SUCCESS;
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
            return Fail(L"process not found: " + options->process);

        dwProcessID = *found;
    }

    wchar_t lpFullDLLPath[MAX_PATH];
    const DWORD dwFullPathResult = GetFullPathNameW(options->dllPath.c_str(), MAX_PATH, lpFullDLLPath, nullptr);
    if (dwFullPathResult == 0 || dwFullPathResult >= MAX_PATH)
        return Fail(L"could not resolve the DLL path: " + injector::FormatLastError());

    std::wprintf(L"[DLL Injector]\n");
    std::wprintf(L"Process    : %ls\n", options->process.c_str());
    std::wprintf(L"Process ID : %lu\n", static_cast<unsigned long>(dwProcessID));
    std::wprintf(L"DLL        : %ls\n\n", lpFullDLLPath);

    const injector::InjectionResult result = injector::InjectDll(dwProcessID, lpFullDLLPath);
    if (!result.success)
        return Fail(result.message);

    std::wprintf(L"%ls\n", result.message.c_str());
    return EXIT_SUCCESS;
}

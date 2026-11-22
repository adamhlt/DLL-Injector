#pragma once

#include <Windows.h>

#include <chrono>
#include <string>

namespace injector
{
    /**
     * Outcome of an injection attempt.
     */
    struct InjectionResult
    {
        bool success = false;     ///< True when the DLL was loaded by the target process.
        HMODULE module = nullptr; ///< Module handle returned by LoadLibrary (null on failure).
        std::wstring message;     ///< Human-readable description of the outcome.
    };

    /**
     * Injects a DLL into a running process using the LoadLibrary technique.
     *
     * The call blocks until the remote LoadLibrary call returns or the timeout
     * elapses. The injector and the target process must have the same bitness.
     *
     * \param processId : id of the target process.
     * \param dllPath : absolute path of the DLL to load.
     * \param timeout : maximum time to wait for LoadLibrary to return.
     * \return : the outcome of the operation.
     */
    InjectionResult InjectDll(DWORD processId, const std::wstring& dllPath,
                              std::chrono::milliseconds timeout = std::chrono::seconds(10));
} // namespace injector

#pragma once

#include <Windows.h>

#include <string>

namespace injector
{
    /**
     * Outcome of an injection attempt.
     */
    struct InjectionResult
    {
        bool success = false;    ///< True when the injection request succeeded.
        std::wstring message;    ///< Human-readable description of the outcome.
    };

    /**
     * Injects a DLL into a running process using the LoadLibrary technique.
     * \param processId : id of the target process.
     * \param dllPath : absolute path of the DLL to load.
     * \return : the outcome of the operation.
     */
    InjectionResult InjectDll(DWORD processId, const std::wstring& dllPath);
} // namespace injector

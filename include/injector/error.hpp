#pragma once

#include <Windows.h>

#include <string>

namespace injector
{
    /**
     * Returns a readable description of a Win32 error code.
     * \param errorCode : code returned by GetLastError().
     * \return : the system message, or a generic text when it cannot be resolved.
     */
    std::wstring FormatError(DWORD errorCode);

    /**
     * Returns a readable description of the last Win32 error (GetLastError()).
     */
    std::wstring FormatLastError();
} // namespace injector

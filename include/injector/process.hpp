#pragma once

#include <Windows.h>

#include <optional>
#include <string_view>

namespace injector
{
    /**
     * Finds the id of the process whose executable name matches.
     * \param processName : executable name to look for (case-insensitive).
     * \return : the process id, or std::nullopt when no process matches.
     */
    std::optional<DWORD> FindProcessId(std::wstring_view processName);
} // namespace injector

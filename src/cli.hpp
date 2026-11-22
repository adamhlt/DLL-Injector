#pragma once

#include <Windows.h>

#include <optional>
#include <string>

namespace injector::cli
{
    /**
     * Parsed command-line options.
     */
    struct Options
    {
        std::wstring dllPath;            ///< DLL path exactly as given by the user.
        std::wstring process;            ///< Target name or numeric id as given.
        std::optional<DWORD> processId;  ///< Set when `process` is a numeric id.
        bool help = false;               ///< True when a help flag was passed.
    };

    /**
     * Parses the command line (argv[0] is ignored).
     * \param error : filled with a message when parsing fails.
     * \return : the parsed options, or std::nullopt on a usage error.
     */
    std::optional<Options> Parse(int argc, wchar_t** argv, std::wstring& error);

    /**
     * Returns the usage/help text.
     */
    std::wstring Usage();
} // namespace injector::cli

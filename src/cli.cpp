#include "cli.hpp"

#include <exception>

namespace injector::cli
{
    namespace
    {
        bool IsHelpFlag(const std::wstring& value)
        {
            return value == L"-h" || value == L"--help" || value == L"/?";
        }

        bool IsNumeric(const std::wstring& value)
        {
            if (value.empty())
                return false;

            for (const wchar_t c : value)
            {
                if (c < L'0' || c > L'9')
                    return false;
            }

            return true;
        }
    } // namespace

    std::wstring Usage()
    {
        return L"Usage: inject.exe <dll> <process>\n"
               L"  <dll>      Path to the DLL to inject.\n"
               L"  <process>  Target executable name (e.g. notepad.exe) or a numeric PID.\n"
               L"  -h, --help Show this help message.\n";
    }

    std::optional<Options> Parse(const int argc, wchar_t** argv, std::wstring& error)
    {
        Options options;

        if (argc >= 2 && IsHelpFlag(argv[1]))
        {
            options.help = true;
            return options;
        }

        if (argc != 3)
        {
            error = L"expected exactly two arguments";
            return std::nullopt;
        }

        options.dllPath = argv[1];
        options.process = argv[2];

        if (IsNumeric(options.process))
        {
            try
            {
                options.processId = static_cast<DWORD>(std::stoul(options.process));
            }
            catch (const std::exception&)
            {
                error = L"invalid process id: " + options.process;
                return std::nullopt;
            }
        }

        return options;
    }
} // namespace injector::cli

#include "process.hpp"

#include "unique_handle.hpp"

#include <TlHelp32.h>

#include <string>

namespace injector
{
    std::optional<DWORD> FindProcessId(const std::wstring_view processName)
    {
        const std::wstring name(processName);

        UniqueHandle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
        if (!snapshot.isValid())
            return std::nullopt;

        PROCESSENTRY32W procEntry{};
        procEntry.dwSize = sizeof(procEntry);

        for (BOOL hasEntry = Process32FirstW(snapshot.get(), &procEntry); hasEntry;
             hasEntry = Process32NextW(snapshot.get(), &procEntry))
        {
            if (lstrcmpiW(procEntry.szExeFile, name.c_str()) == 0)
                return procEntry.th32ProcessID;
        }

        return std::nullopt;
    }
} // namespace injector

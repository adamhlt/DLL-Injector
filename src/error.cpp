#include "injector/error.hpp"

namespace injector
{
    std::wstring FormatError(const DWORD errorCode)
    {
        LPWSTR buffer = nullptr;
        const DWORD length = FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<LPWSTR>(&buffer), 0, nullptr);

        if (length == 0 || buffer == nullptr)
            return L"unknown error (" + std::to_wstring(errorCode) + L")";

        std::wstring message(buffer, length);
        LocalFree(buffer);

        while (!message.empty() && (message.back() == L'\r' || message.back() == L'\n' || message.back() == L' '))
            message.pop_back();

        return message;
    }

    std::wstring FormatLastError()
    {
        return FormatError(GetLastError());
    }
} // namespace injector

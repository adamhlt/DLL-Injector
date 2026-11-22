#pragma once

#include <Windows.h>

namespace injector
{
    /**
     * RAII wrapper that closes a Win32 handle when it goes out of scope.
     *
     * Handles both conventions used by the Win32 API: functions returning
     * NULL (OpenProcess, CreateRemoteThread) and functions returning
     * INVALID_HANDLE_VALUE (CreateToolhelp32Snapshot).
     */
    class UniqueHandle
    {
    public:
        explicit UniqueHandle(const HANDLE handle = nullptr) : m_handle(handle) {}

        ~UniqueHandle()
        {
            if (isValid())
                CloseHandle(m_handle);
        }

        UniqueHandle(const UniqueHandle&) = delete;
        UniqueHandle& operator=(const UniqueHandle&) = delete;

        HANDLE get() const { return m_handle; }
        bool isValid() const { return m_handle != nullptr && m_handle != INVALID_HANDLE_VALUE; }

    private:
        HANDLE m_handle;
    };
} // namespace injector

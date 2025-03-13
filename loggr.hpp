#pragma once
#include <windows.h>
#include <iostream>
#include <chrono>
#define FMT_HEADER_ONLY
#include "fmt/core.h"
#include "fmt/printf.h"

namespace loggr
{
    inline void log(std::string_view message)
    {
        fmt::print("[+] {}\n", message);
    }

    inline void log(const char* message)
    {
        fmt::print("[+] {}\n", message);
    }

    inline void log_error(std::string_view message)
    {
        fmt::print("[!] {}\n", message);
    }

    inline void allocate()
    {
        if (!AllocConsole())
            return;

        if (freopen("CONOUT$", "w", stdout) == nullptr) {
            loggr::log_error("Failed to reopen stdout");
        } else {
            loggr::log("Console allocated successfully");
        }
    }

    inline void free()
    {
        FreeConsole();
    }

    template <bool hex = false, class T>
    inline void log_value(std::string_view variable_name, const T& value)
    {
        constexpr auto format_string = hex ? 
            "[=] {:<15} {:X}\n" :
            "[=] {:<15} {}\n";

        fmt::print(format_string, variable_name, value);
    }

    template <std::size_t indentation>
    inline void log_error_indented(std::string_view message)
    {
        fmt::print("[!] {:<{}}{}\n", ' ', indentation, message);
    }

    template <std::size_t indentation>
    inline void log_indented(std::string_view message)
    {
        fmt::print("[+] {:<{}}{}\n", ' ', indentation, message);
    }

    template <std::size_t indentation, bool hex = false, class T>
    inline void log_indented(std::string_view variable_name, const T& value)
    {
        constexpr auto format_string = hex ?
            "[=] {:<{}}{:.<15} {:02X}\n" :
            "[=] {:<{}}{:.<15} {}\n";

        fmt::print(format_string, ' ', indentation, variable_name, value);
    }

    template <class... T, typename = std::enable_if_t<sizeof...(T) != 0>>
    inline void log_raw(fmt::format_string<T...> format, T&&... arguments)
    {
        fmt::print(format, std::forward<T>(arguments)...);
    }

    inline COORD get_position()
    {
        CONSOLE_SCREEN_BUFFER_INFO info;
        if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info))
        {
            loggr::log_error("Failed to get cursor position");
            return { 0, 0 };
        }

        return info.dwCursorPosition;
    }

    inline void set_position(const COORD cursor)
    {
        if (!SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor))
        {
            loggr::log_error("Failed to set cursor position");
        }
    }

    inline void clear_line()
    {
        auto position = loggr::get_position();

        position.X = 0;

        DWORD count = 0;
        const auto handle = GetStdHandle(STD_OUTPUT_HANDLE);

        CONSOLE_SCREEN_BUFFER_INFO info;
        GetConsoleScreenBufferInfo(handle, &info);
        auto width = info.dwSize.X;

        FillConsoleOutputCharacter(handle, ' ', width, position, &count);

        set_position(position);
    }
};

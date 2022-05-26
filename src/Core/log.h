#pragma once
#include <cstdio>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <string_view>

enum struct LogLevel { DEBUG = 0, INFO = 1, WARNING = 2, ERROR = 3, QUIET = 4 };

template <typename... Args> inline void Debug(std::string_view format, Args... args)
{
		#ifdef DEBUG_MODE
		fmt::print(fmt::fg(fmt::color::light_blue), "[DEBUG]   ");
		fmt::print(fmt::fg(fmt::color::light_blue), format, args...);
		#endif
}

template <typename... Arg>  inline void Info(std::string_view format, Arg... args)
{
		fmt::print(fmt::fg(fmt::color::white), "[INFO]    ");
		fmt::print(fmt::fg(fmt::color::white), format, args...);
}

template <typename... Args> inline void Warn(std::string_view format, Args... args)
{
		fmt::print(fmt::fg(fmt::detail::color_type(0xfefe85)), "[WARNING] ");
		fmt::print(fmt::fg(fmt::detail::color_type(0xfefe85)), format, args...);
}

template <typename... Args> inline void Error(std::string_view format, Args... args)
{
		fmt::print(stderr, fmt::fg(fmt::color::red), "[ERROR]   ");
		fmt::print(stderr, fmt::fg(fmt::color::red), format, args...);
}

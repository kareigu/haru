#pragma once
#include "error.h"
#include <fmt/color.h>
#include <fmt/core.h>
#include <utility>

namespace haru {
namespace log {
  enum class Level {
    Error,
    Warn,
    Info,
    Debug,
  };

  namespace {
    constexpr auto ltoi(Level level) {
      return std::to_underlying(level);
    }

  }// namespace
  Level level();
  void set_level(Level level);

  template<typename... T>
  constexpr void info(fmt::format_string<T...> format_string, T&&... args) {
    if (ltoi(level()) < ltoi(Level::Info))
      return;
    fmt::println(stdout, format_string, std::forward<T>(args)...);
  }

  template<typename... T>
  constexpr void warn(fmt::format_string<T...> format_string, T&&... args) {
    if (ltoi(level()) < ltoi(Level::Warn))
      return;
    fmt::print(stderr, fmt::emphasis::bold | fmt::fg(fmt::color::orange), "[warn] ");
    fmt::println(format_string, std::forward<T>(args)...);
  }

  void error(Error error);

  template<typename... T>
  constexpr void error(fmt::format_string<T...> format_string, T&&... args) {
    if (ltoi(level()) < ltoi(Level::Error))
      return;
    Error err(Error::UnknownError, fmt::format(format_string, std::forward<T>(args)...));
    error(err);
  }

  template<typename... T>
  constexpr void debug(fmt::format_string<T...> format_string, T&&... args) {
    if (ltoi(level()) < ltoi(Level::Debug))
      return;
    fmt::print(stderr, fmt::emphasis::bold | fmt::fg(fmt::color::yellow), "[debug] ");
    fmt::println(format_string, std::forward<T>(args)...);
  }

}// namespace log
}// namespace haru

#include "log.h"
#include "error.h"
#include <cstdio>
#include <fmt/color.h>
#include <fmt/core.h>

namespace haru {
namespace log {
  static Level s_level = Level::INFO;

  Level level() {
    return s_level;
  }

  void set_level(Level level) {
    s_level = level;
  }

  void warn(Error error) {
    if (ltoi(level()) < ltoi(Level::ERROR))
      return;
    fmt::print(stderr, fmt::emphasis::bold | fmt::fg(fmt::color::orange), "[{:s}] ", Error::type_as_string(error.type()));
    fmt::println(stderr, "{:s}", error.message().value_or(""));
  }

  void error(Error error) {
    if (ltoi(level()) < ltoi(Level::ERROR))
      return;
    fmt::print(stderr, fmt::emphasis::bold | fmt::fg(fmt::color::red), "[{:s}] ", Error::type_as_string(error.type()));
    fmt::println(stderr, "{:s}", error.message().value_or(""));
  }
}// namespace log
}// namespace haru

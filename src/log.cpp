#include "log.h"

namespace haru {
namespace log {
  static Level s_level = Level::Info;

  Level level() {
    return s_level;
  }

  void set_level(Level level) {
    s_level = level;
  }

  void error(Error error) {
    if (ltoi(level()) < ltoi(Level::Error))
      return;
    fmt::print(stderr, fmt::emphasis::bold | fmt::fg(fmt::color::red), "[{:s}] ", Error::type_as_string(error.type()));
    fmt::println(stderr, "{:s}", error.message().value_or(""));
  }
}// namespace log
}// namespace haru

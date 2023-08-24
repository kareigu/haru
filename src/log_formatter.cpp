#include "log_formatter.h"
#include <fmt/core.h>
#include <memory>
#include <spdlog/common.h>

namespace haru {
LogFormatter::LogFormatter() {
  m_normal_formatter = std::make_unique<spdlog::pattern_formatter>();
  m_normal_formatter->set_pattern("%v");
  m_labeled_formatter = std::make_unique<spdlog::pattern_formatter>();
  m_labeled_formatter->set_pattern("%^[%l]%$ %v");
}

LogFormatter::~LogFormatter() {}

std::unique_ptr<spdlog::formatter> LogFormatter::clone() const {
  return std::make_unique<LogFormatter>();
}

void LogFormatter::format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest) {
  using level = spdlog::level::level_enum;
  switch (msg.level) {
    case level::debug:
    case level::err:
    case level::trace:
    case level::warn:
    case level::critical:
      m_labeled_formatter->format(msg, dest);
      break;
    default:
      m_normal_formatter->format(msg, dest);
      break;
  }
}
}// namespace haru

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
  m_error_formatter = std::make_unique<spdlog::pattern_formatter>();
  m_error_formatter->add_flag<ErrorFlag>('E');
  m_error_formatter->add_flag<ErrorMessage>('v');
  m_error_formatter->set_pattern("%^[%E]%$ %v");
}

LogFormatter::~LogFormatter() {}

std::unique_ptr<spdlog::formatter> LogFormatter::clone() const {
  return std::make_unique<LogFormatter>();
}

void LogFormatter::format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest) {
  using level = spdlog::level::level_enum;
  switch (msg.level) {
    case level::debug:
    case level::trace:
    case level::warn:
    case level::critical:
      m_labeled_formatter->format(msg, dest);
      break;
    case level::err:
      m_error_formatter->format(msg, dest);
      break;
    default:
      m_normal_formatter->format(msg, dest);
      break;
  }
}

void ErrorFlag::format(const spdlog::details::log_msg& msg, const std::tm& time, spdlog::memory_buf_t& dest) {
  auto string = msg.payload;
  size_t first = 0, last = 0;
  for (size_t i = 0; i < string.size(); i++) {
    if (string[i] == '[')
      first = i + 1;
    if (string[i] == ']') {
      last = i;
      break;
    }
  }
  if (first == last) {
    static const char* err = "UnknownError";
    dest.append(err, err + 13);
  }
  dest.append(string.data() + first, string.data() + last);
}
std::unique_ptr<spdlog::custom_flag_formatter> ErrorFlag::clone() const {
  return spdlog::details::make_unique<ErrorFlag>();
}

void ErrorMessage::format(const spdlog::details::log_msg& msg, const std::tm& time, spdlog::memory_buf_t& dest) {
  auto string = msg.payload;
  size_t start = 0;
  for (size_t i = 0; i < string.size(); i++) {
    if (string[i] == ']') {
      start = i + 1;
      break;
    }
  }
  dest.append(string.data() + start, string.data() + string.size());
}
std::unique_ptr<spdlog::custom_flag_formatter> ErrorMessage::clone() const {
  return spdlog::details::make_unique<ErrorMessage>();
}
}// namespace haru

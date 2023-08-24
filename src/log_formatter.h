#pragma once
#include <memory>
#include <spdlog/common.h>
#include <spdlog/formatter.h>
#include <spdlog/pattern_formatter.h>

namespace haru {
class LogFormatter final : public spdlog::formatter {
public:
  LogFormatter();
  virtual ~LogFormatter() final;

  virtual void format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest) final;
  virtual std::unique_ptr<spdlog::formatter> clone() const final;

private:
  std::unique_ptr<spdlog::pattern_formatter> m_normal_formatter = nullptr;
  std::unique_ptr<spdlog::pattern_formatter> m_labeled_formatter = nullptr;
};
}// namespace haru

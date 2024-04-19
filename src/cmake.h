#pragma once
#include "error.h"
#include <expected>
#include <filesystem>
#include <optional>

namespace haru {
namespace cmake {
  static constexpr const char* COMMAND = "cmake";

  struct Config {
    std::optional<std::string> cpp_compiler = std::nullopt;
    std::optional<std::string> c_compiler = std::nullopt;
    std::string generator;
    std::string build_dir;
  };

  std::expected<void, Error> init(const Config& config, std::optional<const std::filesystem::path> workpath = std::nullopt);
}// namespace cmake
}// namespace haru

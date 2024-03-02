#include "post_ops.h"
#include "error.h"
#include "log.h"
#include "project_info.h"
#include "utils.h"
#include <cstdlib>
#include <expected>
#include <filesystem>
#include <fmt/core.h>
#include <optional>
#include <string_view>


namespace haru {
std::expected<void, Error> handle_git_init(const std::filesystem::path& workpath, bool use_defaults);
std::expected<void, Error> handle_cmake_init(const std::filesystem::path& workpath, Language_t languages, bool use_defaults);
std::optional<std::string_view> get_available_cpp_compiler();
std::optional<std::string_view> get_available_c_compiler();

std::expected<void, Error> handle_post_ops(const std::filesystem::path& workpath, Language_t languages, bool use_defaults) {
  TRY(handle_git_init(workpath, use_defaults));
  TRY(handle_cmake_init(workpath, languages, use_defaults));
  return {};
}

std::expected<void, Error> handle_git_init(const std::filesystem::path& workpath, bool use_defaults) {
  static constexpr const char* GIT_COMMAND = "git";
  if (auto ret = check_command_exists(GIT_COMMAND); !ret) {
    log::warn(ret.error());
    return {};
  }
  log::info("Found {:s} in path", GIT_COMMAND);
  bool init_git = use_defaults ? true : TRY(prompt_yes_no("Initialise git repository?", true));
  if (!init_git) {
    return {};
  }

  if (std::system(fmt::format("cd {} && {:s} init", workpath, GIT_COMMAND).c_str()))
    return std::unexpected(Error(Error::EXEC_ERROR, "Couldn't initialise git"));

  return {};
}

std::expected<void, Error> handle_cmake_init(const std::filesystem::path& workpath, Language_t languages, bool use_defaults) {
  static constexpr const char* CMAKE_COMMAND = "cmake";
  if (auto ret = check_command_exists(CMAKE_COMMAND); !ret) {
    log::warn(ret.error());
    return {};
  }
  log::info("Found {:s} in path", CMAKE_COMMAND);
  bool init_cmake = use_defaults ? true : TRY(prompt_yes_no("Initialise cmake?", true));
  if (!init_cmake) {
    return {};
  }

  std::stringstream cmake_flags;
  if (languages & Language::CPP) {
    auto cpp_compiler_ret = get_available_cpp_compiler();
    if (!cpp_compiler_ret.has_value())
      return std::unexpected(Error(Error::NOT_FOUND, "No valid C++-compiler found in path"));
    std::string cpp_compiler = std::string(cpp_compiler_ret.value());
    if (!use_defaults)
      cpp_compiler = TRY(prompt<std::string>("C++-compiler", cpp_compiler));
    cmake_flags << fmt::format(" -DCMAKE_CXX_COMPILER=\"{:s}\"", cpp_compiler);
  }

  if (languages & Language::C) {
    auto c_compiler_ret = get_available_c_compiler();
    if (!c_compiler_ret.has_value())
      return std::unexpected(Error(Error::NOT_FOUND, "No valid C-compiler found in path"));
    std::string c_compiler = std::string(c_compiler_ret.value());
    if (!use_defaults)
      c_compiler = TRY(prompt<std::string>("C++-compiler", c_compiler));
    cmake_flags << fmt::format(" -DCMAKE_C_COMPILER=\"{:s}\"", c_compiler);
  }
  cmake_flags << " -DCMAKE_EXPORT_COMPILE_COMMANDS=1";

  static constexpr const char* DEFAULT_GENERATOR = "Ninja Multi-Config";
  std::string generator = use_defaults ? DEFAULT_GENERATOR : TRY(prompt<std::string>("Build generator", DEFAULT_GENERATOR));
  cmake_flags << " -G \"" << generator << '"';

  static constexpr const char* DEFAULT_DIR = "build";
  std::string build_dir = use_defaults ? DEFAULT_DIR : TRY(prompt<std::string>("Build directory", DEFAULT_DIR));
  cmake_flags << " -B \"" << build_dir << '"';

  if (std::system(fmt::format("cd {} && {:s} {:s} .", workpath, CMAKE_COMMAND, cmake_flags.str()).c_str()))
    return std::unexpected(Error(Error::EXEC_ERROR, "Couldn't initialise cmake"));

  return {};
}

std::optional<std::string_view> get_available_cpp_compiler() {
  static constexpr const char* CLANG = "clang++";
  static constexpr const char* GCC = "g++";

  if (check_command_exists(CLANG).has_value())
    return CLANG;
  if (check_command_exists(GCC).has_value())
    return GCC;
  return std::nullopt;
}
std::optional<std::string_view> get_available_c_compiler() {
  static constexpr const char* CLANG = "clang";
  static constexpr const char* GCC = "gcc";
  if (check_command_exists(CLANG).has_value())
    return CLANG;
  if (check_command_exists(GCC).has_value())
    return GCC;
  return std::nullopt;
}

}// namespace haru

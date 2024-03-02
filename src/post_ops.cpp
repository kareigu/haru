#include "post_ops.h"
#include "error.h"
#include "log.h"
#include "utils.h"
#include <cstdlib>
#include <expected>
#include <filesystem>


namespace haru {
std::expected<void, Error> handle_git_init(const std::filesystem::path& workpath, bool use_defaults);
std::expected<void, Error> handle_cmake_init(const std::filesystem::path& workpath, bool use_defaults);

std::expected<void, Error> handle_post_ops(const std::filesystem::path& workpath, bool use_defaults) {
  TRY(handle_git_init(workpath, use_defaults));
  TRY(handle_cmake_init(workpath, use_defaults));
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

std::expected<void, Error> handle_cmake_init(const std::filesystem::path& workpath, bool use_defaults) {
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

  if (std::system(fmt::format("cd {} && {:s} -B build .", workpath, CMAKE_COMMAND).c_str()))
    return std::unexpected(Error(Error::EXEC_ERROR, "Couldn't initialise cmake"));

  return {};
}

}// namespace haru

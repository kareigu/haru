#include "post_ops.h"
#include "error.h"
#include "log.h"
#include "utils.h"
#include <cstdlib>
#include <expected>
#include <filesystem>


namespace haru {
std::expected<void, Error> handle_git_init(const std::filesystem::path& workpath, bool use_defaults);

std::expected<void, Error> handle_post_ops(const std::filesystem::path& workpath, bool use_defaults) {
  TRY(handle_git_init(workpath, use_defaults));
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
}// namespace haru

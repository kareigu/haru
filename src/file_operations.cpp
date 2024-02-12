#include "file_operations.h"
#include "utils.h"
#include <fstream>
#include <spdlog/spdlog.h>

namespace haru {
cpp::result<std::filesystem::path, Error> create_work_directory(bool init, const std::string& name) {
  auto workpath = std::filesystem::current_path();
  if (!init) {
    workpath += "/" + name;
    if (std::filesystem::exists(workpath))
      return cpp::fail(Error(Error::AlreadyExists, fmt::format("{:s} already exists at {:s}", name, workpath.string())));

    if (!std::filesystem::create_directory(workpath)) {
      return cpp::fail(Error(Error::IOError, fmt::format("Could not create directory at {:s}", workpath.string())));
    }
    spdlog::info("Created project directory {:s}", workpath.string());
    return workpath;
  }

  if (!std::filesystem::is_empty(workpath)) {
    bool proceed = TRY(prompt_yes_no("Current directory is not empty, proceed?", false));
    if (!proceed)
      return cpp::fail(Error(Error::IOError, "Current directory is not empty"));
  }


  spdlog::info("Using project directory {:s}", workpath.string());
  return workpath;
}

cpp::result<void, Error> write_cmake_lists(const std::filesystem::path& workpath, const std::string& contents) {
  auto filepath = workpath;
  filepath += "/CMakeLists.txt";
  std::ofstream output(filepath);
  output << contents;
  if (output.rdstate() & (std::ofstream::failbit | std::ofstream::badbit))
    return cpp::fail(Error(Error::Write, "Failed writing CMakeLists.txt"));

  spdlog::info("Wrote {}", filepath.string());
  return {};
}
}// namespace haru

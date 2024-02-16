#include "file_operations.h"
#include "bake_in.h"
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

cpp::result<void, Error> write_entry_point(const std::filesystem::path& workpath, const std::string& entry_point, Language_t languages) {
  if (!(languages & Language::cpp) && !(languages & Language::c))
    return cpp::fail(Error(Error::IOError, "Unsupported language"));

  auto full_path = workpath;
  full_path += "/" + entry_point;
  auto folder = full_path;
  folder.remove_filename();
  if (!std::filesystem::create_directory(folder))
    return cpp::fail(Error(Error::IOError, fmt::format("Failed creating directory {:s}", folder.string())));

  std::ofstream output(full_path);
  if (!output.is_open())
    return cpp::fail(Error(Error::IOError, "Could not open entry point file for writing"));

  if (languages & Language::cpp)
    output << "#include <iostream>\n\n";
  else if (languages & Language::c)
    output << "#include <stdio.h>\n\n";

  output << "int main() {\n";
  if (languages & Language::cpp)
    output << "  std::cout << \"" HARU_PRG_NAME "\" << std::endl;\n\n";
  else if (languages & Language::c)
    output << "  printf(\"" HARU_PRG_NAME "\n\");\n\n";

  output << "  return 0;\n";
  output << "}";

  if (output.fail() || output.bad())
    return cpp::fail(Error(Error::Write, fmt::format("Failed writing {:s}", full_path.string())));

  spdlog::info("Wrote {:s}", full_path.string());
  return {};
}

cpp::result<void, Error> write_default_files(const std::filesystem::path& workpath, DefaultFiles_t default_files) {
  if (default_files & DefaultFiles::clang_format) {
    auto filepath = workpath;
    filepath += "/.clang-format";
    std::ofstream output(filepath);
    for (size_t i = 0; i < bake_in_clang_format_len; i++)
      output << static_cast<char>(bake_in_clang_format[i]);
    if (output.bad() || output.fail())
      return cpp::fail(Error(Error::Write, "Failed writing .clang-format"));
    spdlog::info("Wrote {:s}", filepath.string());
  }
  if (default_files & DefaultFiles::cmake_format) {
    auto filepath = workpath;
    filepath += "/.cmake-format.py";
    std::ofstream output(filepath);
    for (size_t i = 0; i < bake_in_cmake_format_len; i++)
      output << static_cast<char>(bake_in_cmake_format[i]);
    if (output.bad() || output.fail())
      return cpp::fail(Error(Error::Write, "Failed writing .cmake_format.py"));
    spdlog::info("Wrote {:s}", filepath.string());
  }
  if (default_files & DefaultFiles::gitignore) {
    auto filepath = workpath;
    filepath += "/.gitignore";
    std::ofstream output(filepath);
    for (size_t i = 0; i < bake_in_gitignore_len; i++)
      output << static_cast<char>(bake_in_gitignore[i]);
    if (output.bad() || output.fail())
      return cpp::fail(Error(Error::Write, "Failed writing .gitignore"));
    spdlog::info("Wrote {:s}", filepath.string());
  }
  return {};
}
}// namespace haru

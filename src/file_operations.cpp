#include "file_operations.h"
#include "bake_in.h"
#include "error.h"
#include "log.h"
#include "project_info.h"
#include "utils.h"
#include <cstddef>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <result.hpp>
#include <string>
#include <system_error>

namespace haru {
cpp::result<std::filesystem::path, Error> create_work_directory(bool init, const std::string& name, bool overwrite) {
  auto workpath = std::filesystem::current_path();
  if (!init) {
    workpath += "/" + name;
    if (std::filesystem::exists(workpath)) {
      if (!overwrite)
        return cpp::fail(Error(Error::ALREADY_EXISTS, fmt::format("{:s} already exists at {:s}. Use '--force' to overwrite", name, workpath.string())));

      std::error_code ec;
      std::filesystem::remove_all(workpath, ec);
      if (ec)
        return cpp::fail(Error(
                Error::IO_ERROR,
                fmt::format("Could not remove existing directory at {:s}, {:s}", workpath.string(), ec.message())));
    }

    if (!std::filesystem::create_directory(workpath)) {
      return cpp::fail(Error(Error::IO_ERROR, fmt::format("Could not create directory at {:s}", workpath.string())));
    }
    log::info("Created project directory {:s}", workpath.string());
    return workpath;
  }

  if (!std::filesystem::is_empty(workpath)) {
    bool proceed = TRY(prompt_yes_no("Current directory is not empty, proceed?", false));
    if (!proceed)
      return cpp::fail(Error(Error::IO_ERROR, "Current directory is not empty"));
  }


  log::info("Using project directory {:s}", workpath.string());
  return workpath;
}

cpp::result<void, Error> write_cmake_lists(const std::filesystem::path& workpath, const std::string& contents) {
  auto filepath = workpath;
  filepath += "/CMakeLists.txt";
  std::ofstream output(filepath);
  output << contents;
  if (output.fail() || output.bad())
    return cpp::fail(Error(Error::WRITE_ERROR, "Failed writing CMakeLists.txt"));

  log::info("Wrote {}", filepath.string());
  return {};
}

cpp::result<void, Error> write_entry_point(const std::filesystem::path& workpath, const std::string& entry_point, Language_t languages) {
  if (!(languages & Language::CPP) && !(languages & Language::C))
    return cpp::fail(Error(Error::IO_ERROR, "Unsupported language"));

  auto full_path = workpath;
  full_path += "/" + entry_point;
  auto folder = full_path;
  folder.remove_filename();
  if (!std::filesystem::create_directory(folder))
    return cpp::fail(Error(Error::IO_ERROR, fmt::format("Failed creating directory {:s}", folder.string())));

  std::ofstream output(full_path);
  if (!output.is_open())
    return cpp::fail(Error(Error::IO_ERROR, "Could not open entry point file for writing"));

  if (languages & Language::CPP)
    output << "#include <iostream>\n\n";
  else if (languages & Language::C)
    output << "#include <stdio.h>\n\n";

  output << "int main() {\n";
  if (languages & Language::CPP)
    output << "  std::cout << \"" HARU_PRG_NAME "\" << std::endl;\n\n";
  else if (languages & Language::C)
    output << "  printf(\"" HARU_PRG_NAME "\\n\");\n\n";

  output << "  return 0;\n";
  output << "}";

  if (output.fail() || output.bad())
    return cpp::fail(Error(Error::WRITE_ERROR, fmt::format("Failed writing {:s}", full_path.string())));

  log::info("Wrote {:s}", full_path.string());
  return {};
}

cpp::result<void, Error> write_default_files(const std::filesystem::path& workpath, DefaultFiles_t default_files) {
  if (default_files & DefaultFiles::CLANG_FORMAT) {
    auto filepath = workpath;
    filepath += "/.clang-format";
    std::ofstream output(filepath);
    for (size_t i = 0; i < bake_in_clang_format_len; i++)
      output << static_cast<char>(bake_in_clang_format[i]);
    if (output.bad() || output.fail())
      return cpp::fail(Error(Error::WRITE_ERROR, "Failed writing .clang-format"));
    log::info("Wrote {:s}", filepath.string());
  }
  if (default_files & DefaultFiles::CLANG_FORMAT) {
    auto filepath = workpath;
    filepath += "/.cmake-format.py";
    std::ofstream output(filepath);
    for (size_t i = 0; i < bake_in_cmake_format_len; i++)
      output << static_cast<char>(bake_in_cmake_format[i]);
    if (output.bad() || output.fail())
      return cpp::fail(Error(Error::WRITE_ERROR, "Failed writing .cmake_format.py"));
    log::info("Wrote {:s}", filepath.string());
  }
  if (default_files & DefaultFiles::GITIGNORE) {
    auto filepath = workpath;
    filepath += "/.gitignore";
    std::ofstream output(filepath);
    for (size_t i = 0; i < bake_in_gitignore_len; i++)
      output << static_cast<char>(bake_in_gitignore[i]);
    if (output.bad() || output.fail())
      return cpp::fail(Error(Error::WRITE_ERROR, "Failed writing .gitignore"));
    log::info("Wrote {:s}", filepath.string());
  }
  return {};
}
}// namespace haru

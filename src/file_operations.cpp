#include "file_operations.h"
#include "bake_in.h"
#include "cmake_lists_generator.h"
#include "error.h"
#include "log.h"
#include "project_info.h"
#include "utils.h"
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#ifdef _WIN32
  #include <execution>
#endif
#include <expected>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <string>
#include <system_error>
#include <vector>

namespace haru {
namespace file_ops {
  std::expected<std::filesystem::path, Error> create_work_directory(bool init, const std::string& name, bool overwrite) {
    auto workpath = std::filesystem::current_path();
    if (!init) {
      workpath += "/" + name;
      if (std::filesystem::exists(workpath)) {
        if (!overwrite)
          return std::unexpected(Error(
                  Error::ALREADY_EXISTS,
                  fmt::format("{:s} already exists at {}. Use '--force' to overwrite", name, workpath)));

        std::error_code ec;
        std::filesystem::remove_all(workpath, ec);
        if (ec)
          return std::unexpected(Error(
                  Error::IO_ERROR,
                  fmt::format("Could not remove existing directory at {}, {:s}", workpath, ec.message())));
      }

      if (!std::filesystem::create_directory(workpath)) {
        return std::unexpected(Error(Error::IO_ERROR, fmt::format("Could not create directory at {}", workpath)));
      }
      log::info("Created project directory {}", workpath);
      return workpath;
    }

    if (!std::filesystem::is_empty(workpath)) {
      bool proceed = overwrite ? true : TRY(prompt_yes_no("Current directory is not empty, proceed?", false));
      if (!proceed)
        return std::unexpected(Error(Error::IO_ERROR, "Current directory is not empty"));
    }


    log::info("Using project directory {}", workpath);
    return workpath;
  }

  std::expected<void, Error> write_cmake_files(const std::filesystem::path& workpath, const std::vector<CMakeListsGenerator::CMakeFile>& files) {
    for (const auto& file : files) {
      std::filesystem::path filepath = workpath;
      if (file.filepath.has_parent_path()) {
        std::filesystem::path file_directory = workpath;
        file_directory += "/";
        file_directory += file.filepath.parent_path();
        if (!std::filesystem::exists(file_directory) && !std::filesystem::create_directories(file_directory))
          return std::unexpected(Error(Error::IO_ERROR, fmt::format("Couldn't create directories for path {}", file_directory)));
      }
      filepath += "/";
      filepath += file.filepath;
      std::ofstream output(filepath);
      output << file.contents;
      if (output.fail() || output.bad())
        return std::unexpected(Error(Error::WRITE_ERROR, fmt::format("Failed writing {}", filepath)));

      log::info("Wrote {}", filepath);
    }
    return {};
  }

  std::expected<void, Error> write_entry_point(const std::filesystem::path& workpath, const std::string& entry_point, Language_t languages) {
    if (!(languages & Language::CPP) && !(languages & Language::C))
      return std::unexpected(Error(Error::IO_ERROR, "Unsupported language"));

    auto full_path = workpath;
    full_path += "/" + entry_point;
    auto directory = full_path;
    directory.remove_filename();
    if (!std::filesystem::exists(directory) && !std::filesystem::create_directory(directory))
      return std::unexpected(Error(Error::IO_ERROR, fmt::format("Failed creating directory {}", directory)));

    std::ofstream output(full_path);
    if (!output.is_open())
      return std::unexpected(Error(Error::IO_ERROR, "Could not open entry point file for writing"));

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
      return std::unexpected(Error(Error::WRITE_ERROR, fmt::format("Failed writing {}", full_path)));

    log::info("Wrote {}", full_path);
    return {};
  }

  std::expected<void, Error> write_default_files(const std::filesystem::path& workpath, DefaultFiles_t default_files) {
    if (default_files & DefaultFiles::CLANG_FORMAT) {
      auto filepath = workpath;
      filepath += "/.clang-format";
      std::ofstream output(filepath);
      for (size_t i = 0; i < bake_in_clang_format_len; i++)
        output << static_cast<char>(bake_in_clang_format[i]);
      if (output.bad() || output.fail())
        return std::unexpected(Error(Error::WRITE_ERROR, "Failed writing .clang-format"));
      log::info("Wrote {}", filepath);
    }
    if (default_files & DefaultFiles::GERSEMIRC) {
      auto filepath = workpath;
      filepath += "/.gersemirc";
      std::ofstream output(filepath);
      for (size_t i = 0; i < bake_in_gersemirc_len; i++)
        output << static_cast<char>(bake_in_gersemirc[i]);
      if (output.bad() || output.fail())
        return std::unexpected(Error(Error::WRITE_ERROR, "Failed writing .gersemirc"));
      log::info("Wrote {}", filepath);
    }
    if (default_files & DefaultFiles::GITIGNORE) {
      auto filepath = workpath;
      filepath += "/.gitignore";
      std::ofstream output(filepath);
      for (size_t i = 0; i < bake_in_gitignore_len; i++)
        output << static_cast<char>(bake_in_gitignore[i]);
      if (output.bad() || output.fail())
        return std::unexpected(Error(Error::WRITE_ERROR, "Failed writing .gitignore"));
      log::info("Wrote {}", filepath);
    }
    return {};
  }

  std::expected<void, Error> format_generated_files(const std::filesystem::path& workpath, FormatGeneratedFilesArgs args) noexcept {
    if (args.default_files & DefaultFiles::GERSEMIRC)
      TRY(format_cmake_files(workpath, args.files));

    if (args.default_files & DefaultFiles::CLANG_FORMAT)
      TRY(format_source_files(workpath, args.source_path));

    return {};
  }

  std::expected<void, Error> format_cmake_files(const std::filesystem::path& workpath, const std::vector<CMakeListsGenerator::CMakeFile>& files) {
    static constexpr const char* CMAKE_FORMATTER = "gersemi";
    TRY(check_command_exists(CMAKE_FORMATTER));
    log::info("Found {:s} to run formatting", CMAKE_FORMATTER);

    std::optional<Error> failed = std::nullopt;
    std::for_each(
#ifdef _WIN32
            std::execution::parallel_policy(),
#endif
            files.begin(), files.end(),
            [&workpath, &failed](const CMakeListsGenerator::CMakeFile& file) {
              auto filepath = file.filepath.string();
              if (std::system(fmt::format("cd {} && {:s} -i {}", workpath, CMAKE_FORMATTER, filepath).c_str())) {
                failed = Error(Error::EXEC_ERROR, fmt::format("Could not format {:s} using {}", filepath, CMAKE_FORMATTER));
                return;
              }
              log::info("Formatted {}/{}", workpath, filepath);
            });

    if (failed.has_value())
      return std::unexpected(failed.value());

    return {};
  }

  std::expected<void, Error> format_source_files(const std::filesystem::path& workpath, const std::string& source_path) {
    static constexpr const char* SOURCE_FORMATTER = "clang-format";
    TRY(check_command_exists(SOURCE_FORMATTER));
    log::info("Found {:s} to run formatting", SOURCE_FORMATTER);
    if (std::system(fmt::format("cd {} && {:s} -i {:s}", workpath, SOURCE_FORMATTER, source_path).c_str()))
      return std::unexpected(Error(Error::EXEC_ERROR, fmt::format("Could not format {:s} using {:s}", source_path, SOURCE_FORMATTER)));

    log::info("Formatted {}/{:s}", workpath, source_path);
    return {};
  }
}// namespace file_ops
}// namespace haru

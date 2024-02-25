#include "file_operations.h"
#include "bake_in.h"
#include "cmake_lists_generator.h"
#include "error.h"
#include "log.h"
#include "project_info.h"
#include "utils.h"
#include <cstddef>
#include <cstdlib>
#include <expected>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <string>
#include <system_error>

namespace haru {
namespace file_ops {
  std::expected<std::filesystem::path, Error> create_work_directory(bool init, const std::string& name, bool overwrite) {
    auto workpath = std::filesystem::current_path();
    if (!init) {
      workpath += "/" + name;
      if (std::filesystem::exists(workpath)) {
        if (!overwrite)
          return std::unexpected(Error(Error::ALREADY_EXISTS, fmt::format("{:s} already exists at {:s}. Use '--force' to overwrite", name, workpath.string())));

        std::error_code ec;
        std::filesystem::remove_all(workpath, ec);
        if (ec)
          return std::unexpected(Error(
                  Error::IO_ERROR,
                  fmt::format("Could not remove existing directory at {:s}, {:s}", workpath.string(), ec.message())));
      }

      if (!std::filesystem::create_directory(workpath)) {
        return std::unexpected(Error(Error::IO_ERROR, fmt::format("Could not create directory at {:s}", workpath.string())));
      }
      log::info("Created project directory {:s}", workpath.string());
      return workpath;
    }

    if (!std::filesystem::is_empty(workpath)) {
      bool proceed = overwrite ? true : TRY(prompt_yes_no("Current directory is not empty, proceed?", false));
      if (!proceed)
        return std::unexpected(Error(Error::IO_ERROR, "Current directory is not empty"));
    }


    log::info("Using project directory {:s}", workpath.string());
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
          return std::unexpected(Error(Error::IO_ERROR, fmt::format("Couldn't create directories for path {:s}", file_directory.string())));
      }
      filepath += "/";
      filepath += file.filepath;
      std::ofstream output(filepath);
      output << file.contents;
      if (output.fail() || output.bad())
        return std::unexpected(Error(Error::WRITE_ERROR, fmt::format("Failed writing {:s}", filepath.string())));

      log::info("Wrote {}", filepath.string());
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
      return std::unexpected(Error(Error::IO_ERROR, fmt::format("Failed creating directory {:s}", directory.string())));

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
      return std::unexpected(Error(Error::WRITE_ERROR, fmt::format("Failed writing {:s}", full_path.string())));

    log::info("Wrote {:s}", full_path.string());
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
      log::info("Wrote {:s}", filepath.string());
    }
    if (default_files & DefaultFiles::GERSEMIRC) {
      auto filepath = workpath;
      filepath += "/.gersemirc.py";
      std::ofstream output(filepath);
      for (size_t i = 0; i < bake_in_gersemirc_len; i++)
        output << static_cast<char>(bake_in_gersemirc[i]);
      if (output.bad() || output.fail())
        return std::unexpected(Error(Error::WRITE_ERROR, "Failed writing .gersemirc"));
      log::info("Wrote {:s}", filepath.string());
    }
    if (default_files & DefaultFiles::GITIGNORE) {
      auto filepath = workpath;
      filepath += "/.gitignore";
      std::ofstream output(filepath);
      for (size_t i = 0; i < bake_in_gitignore_len; i++)
        output << static_cast<char>(bake_in_gitignore[i]);
      if (output.bad() || output.fail())
        return std::unexpected(Error(Error::WRITE_ERROR, "Failed writing .gitignore"));
      log::info("Wrote {:s}", filepath.string());
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
    constexpr const char* CMAKE_FORMATTER = "gersemi";
    TRY(check_command_exists(CMAKE_FORMATTER));
    log::info("Found {:s} to run formatting", CMAKE_FORMATTER);

    for (const auto& file : files) {
      std::string filepath = file.filepath.string();
      if (std::system(fmt::format("cd {:s} && {:s} -i {:s}", workpath.string(), CMAKE_FORMATTER, filepath).c_str()))
        return std::unexpected(Error(Error::EXEC_ERROR, fmt::format("Could not format {:s} using {:s}", filepath, CMAKE_FORMATTER)));

      log::info("Formatted {:s}/{:s}", workpath.string(), filepath);
    }
    return {};
  }

  std::expected<void, Error> format_source_files(const std::filesystem::path& workpath, const std::string& source_path) {
    constexpr const char* SOURCE_FORMATTER = "clang-format";
    TRY(check_command_exists(SOURCE_FORMATTER));
    log::info("Found {:s} to run formatting", SOURCE_FORMATTER);
    if (std::system(fmt::format("cd {:s} && {:s} -i {:s}", workpath.string(), SOURCE_FORMATTER, source_path).c_str()))
      return std::unexpected(Error(Error::EXEC_ERROR, fmt::format("Could not format {:s} using {:s}", source_path, SOURCE_FORMATTER)));

    log::info("Formatted {:s}/{:s}", workpath.string(), source_path);
    return {};
  }
}// namespace file_ops
}// namespace haru

#pragma once
#include "cmake_lists_generator.h"
#include "error.h"
#include "project_info.h"
#include <expected>
#include <filesystem>
#include <string>

namespace haru {
namespace file_ops {
  std::expected<std::filesystem::path, Error> create_work_directory(bool init, const std::string& name, bool overwrite);
  std::expected<void, Error> write_cmake_files(const std::filesystem::path& workpath, const std::vector<CMakeListsGenerator::CMakeFile>& files);
  std::expected<void, Error> write_entry_point(const std::filesystem::path& workpath, const std::string& entry_point, Language_t languages);
  std::expected<void, Error> write_default_files(const std::filesystem::path& workpath, DefaultFiles_t default_files);
  std::expected<void, Error> format_cmake_files(const std::filesystem::path& workpath, const std::vector<CMakeListsGenerator::CMakeFile>& files);
}// namespace file_ops
}// namespace haru

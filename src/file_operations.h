#pragma once
#include "error.h"
#include "project_info.h"
#include <expected>
#include <filesystem>
#include <string>

namespace haru {
std::expected<std::filesystem::path, Error> create_work_directory(bool init, const std::string& name, bool overwrite);
std::expected<void, Error> write_cmake_lists(const std::filesystem::path& workpath, const std::string& contents);
std::expected<void, Error> write_entry_point(const std::filesystem::path& workpath, const std::string& entry_point, Language_t languages);
std::expected<void, Error> write_default_files(const std::filesystem::path& workpath, DefaultFiles_t default_files);
}// namespace haru

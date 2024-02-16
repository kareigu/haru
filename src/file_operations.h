#pragma once
#include "error.h"
#include "project_info.h"
#include <filesystem>
#include <result.hpp>

namespace haru {
cpp::result<std::filesystem::path, Error> create_work_directory(bool init, const std::string& name);
cpp::result<void, Error> write_cmake_lists(const std::filesystem::path& workpath, const std::string& contents);
cpp::result<void, Error> write_default_files(const std::filesystem::path& workpath, DefaultFiles_t default_files);
}// namespace haru

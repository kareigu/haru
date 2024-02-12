#pragma once
#include "error.h"
#include <filesystem>
#include <result.hpp>

namespace haru {
cpp::result<std::filesystem::path, Error> create_work_directory(bool init, const std::string& name);
cpp::result<void, Error> write_cmake_lists(const std::filesystem::path& workpath, const std::string& contents);
}// namespace haru

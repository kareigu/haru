#pragma once
#include "error.h"
#include <filesystem>
#include <result.hpp>

namespace haru {
cpp::result<void, Error> write_cmake_lists(const std::filesystem::path& workpath, const std::string& contents);
}

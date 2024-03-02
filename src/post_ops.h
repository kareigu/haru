#pragma once
#include "error.h"
#include "project_info.h"
#include <expected>
#include <filesystem>

namespace haru {
std::expected<void, Error> handle_post_ops(const std::filesystem::path& workpath, Language_t languages, bool use_defaults);
}// namespace haru

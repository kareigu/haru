#pragma once
#include "error.h"
#include <expected>
#include <filesystem>

namespace haru {
std::expected<void, Error> handle_post_ops(const std::filesystem::path& workpath, bool use_defaults);
}// namespace haru

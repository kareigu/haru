#pragma once
#include "command.h"
#include "error.h"
#include <expected>
#include <filesystem>
#include <fmt/core.h>
#include <fmt/std.h>
#include <optional>

namespace haru {
namespace cmake {
static constexpr const char* COMMAND = "cmake";

struct Config {
    std::optional<std::string> cpp_compiler = std::nullopt;
    std::optional<std::string> c_compiler = std::nullopt;
    std::string generator;
    std::string build_dir;
};

std::expected<void, Error>
init(const Config& config,
     std::optional<const std::filesystem::path> workpath = std::nullopt);
std::expected<void, Error> handle_init_command(const Command::Flags_t flags);
} // namespace cmake
} // namespace haru

template <> struct fmt::formatter<haru::cmake::Config> {
    template <typename ParseContext> constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    auto format(haru::cmake::Config config, format_context& ctx) const {
        fmt::format_to(ctx.out(), "cpp_compiler = {:s}\n",
                       config.cpp_compiler ? config.cpp_compiler.value()
                                           : "None");
        fmt::format_to(ctx.out(), "c_compiler = {:s}\n",
                       config.c_compiler ? config.c_compiler.value() : "None");
        fmt::format_to(ctx.out(), "generator = {:s}\n", config.generator);
        fmt::format_to(ctx.out(), "build_dir = {:s}\n", config.build_dir);
        return ctx.out();
    }
};

#include "post_ops.h"
#include "cmake.h"
#include "config.h"
#include "error.h"
#include "log.h"
#include "project_info.h"
#include "utils.h"
#include <cstdlib>
#include <expected>
#include <filesystem>
#include <fmt/core.h>
#include <optional>
#include <string_view>

namespace haru {
std::expected<void, Error>
handle_git_init(const std::filesystem::path& workpath, bool use_defaults);
std::expected<void, Error>
handle_cmake_init(const std::filesystem::path& workpath, Language_t languages,
                  bool use_defaults);
std::optional<std::string> get_available_cpp_compiler();
std::optional<std::string> get_available_c_compiler();
std::optional<std::string> get_available_generator();

std::expected<void, Error>
handle_post_ops(const std::filesystem::path& workpath, Language_t languages,
                bool use_defaults) {
    TRY(handle_git_init(workpath, use_defaults));
    TRY(handle_cmake_init(workpath, languages, use_defaults));
    return {};
}

std::expected<void, Error>
handle_git_init(const std::filesystem::path& workpath, bool use_defaults) {
    static constexpr const char* GIT_COMMAND = "git";
    if (auto ret = check_command_exists(GIT_COMMAND); !ret) {
        log::warn(ret.error());
        return {};
    }
    log::info("Found {:s} in path", GIT_COMMAND);
    bool init_git =
        use_defaults ? true
                     : TRY(prompt_yes_no("Initialise git repository?", true));
    if (!init_git) {
        return {};
    }

    if (std::system(
            fmt::format("cd {} && {:s} init", workpath, GIT_COMMAND).c_str()))
        return std::unexpected(
            Error(Error::EXEC_ERROR, "Couldn't initialise git"));

    return {};
}

std::expected<void, Error>
handle_cmake_init(const std::filesystem::path& workpath, Language_t languages,
                  bool use_defaults) {
    if (auto ret = check_command_exists(cmake::COMMAND); !ret) {
        log::warn(ret.error());
        return {};
    }
    log::info("Found {:s} in path", cmake::COMMAND);
    bool init_cmake =
        use_defaults ? true : TRY(prompt_yes_no("Initialise cmake?", true));
    if (!init_cmake) {
        return {};
    }

    cmake::Config config;
    if (languages & Language::CPP) {
        auto cpp_compiler_ret = get_available_cpp_compiler();
        if (!cpp_compiler_ret.has_value())
            return std::unexpected(
                Error(Error::NOT_FOUND, "No valid C++-compiler found in path"));
        std::string cpp_compiler = std::string(cpp_compiler_ret.value());
        if (!use_defaults)
            cpp_compiler =
                TRY(prompt<std::string>("C++-compiler", cpp_compiler));
        config.cpp_compiler = cpp_compiler;
    }

    if (languages & Language::C) {
        auto c_compiler_ret = get_available_c_compiler();
        if (!c_compiler_ret.has_value())
            return std::unexpected(
                Error(Error::NOT_FOUND, "No valid C-compiler found in path"));
        std::string c_compiler = std::string(c_compiler_ret.value());
        if (!use_defaults)
            c_compiler = TRY(prompt<std::string>("C-compiler", c_compiler));
        config.c_compiler = c_compiler;
    }

    auto default_generator = get_available_generator();
    if (use_defaults && default_generator)
        config.generator = default_generator.value();
    else
        config.generator = TRY(prompt<std::string>(
            "Build generator",
            default_generator.transform(
                [](std::string_view v) { return std::string(v); })));

    std::string default_dir = TRY(Config::get_value("cmake.build_dir"));
    std::string build_dir =
        use_defaults ? default_dir
                     : TRY(prompt<std::string>("Build directory", default_dir));
    config.build_dir = build_dir;

    TRY(cmake::init(config, workpath));

    return {};
}

std::optional<std::string> get_available_cpp_compiler() {
    static constexpr const char* CLANG = "clang++";
    static constexpr const char* GCC = "g++";

    auto config_ret = Config::get_value("cmake.cpp_compiler");
    if (config_ret.has_value()) {
        if (check_command_exists(config_ret.value()).has_value())
            return config_ret.value();
    }

    if (check_command_exists(CLANG).has_value())
        return CLANG;
    if (check_command_exists(GCC).has_value())
        return GCC;
    return std::nullopt;
}
std::optional<std::string> get_available_c_compiler() {
    static constexpr const char* CLANG = "clang";
    static constexpr const char* GCC = "gcc";

    auto config_ret = Config::get_value("cmake.c_compiler");
    if (config_ret.has_value()) {
        if (check_command_exists(config_ret.value()).has_value())
            return config_ret.value();
    }

    if (check_command_exists(CLANG).has_value())
        return CLANG;
    if (check_command_exists(GCC).has_value())
        return GCC;
    return std::nullopt;
}

std::optional<std::string> get_available_generator() {
    static constexpr const char* NINJA_MULTICONFIG = "Ninja Multi-Config";
    static constexpr const char* MAKE = "Unix Makefiles";

    auto config_ret = Config::get_value("cmake.generator");
    if (config_ret.has_value()) {
        std::string config_generator = config_ret.value();
        if (config_generator.contains("Ninja"))
            if (check_command_exists("ninja").has_value())
                return config_ret.value();
        if (config_generator.contains("Makefiles"))
            if (check_command_exists("make").has_value())
                return config_ret.value();
    }

    if (check_command_exists("ninja").has_value())
        return NINJA_MULTICONFIG;
    if (check_command_exists("make").has_value())
        return MAKE;
    return std::nullopt;
}

} // namespace haru

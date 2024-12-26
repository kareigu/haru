#include "cmake.h"
#include "config.h"
#include "log.h"
#include "utils.h"
#include <cstdlib>
#include <sstream>

namespace haru {
namespace cmake {
std::expected<void, Error>
init(const Config& config,
     std::optional<const std::filesystem::path> workpath) {
    std::stringstream flags;

    if (config.cpp_compiler)
        flags << fmt::format(" -DCMAKE_CXX_COMPILER=\"{:s}\"",
                             config.cpp_compiler.value());

    if (config.c_compiler)
        flags << fmt::format(" -DCMAKE_C_COMPILER=\"{:s}\"",
                             config.c_compiler.value());

    flags << " -DCMAKE_EXPORT_COMPILE_COMMANDS=1";
    flags << " -G \"" << config.generator << '"';
    flags << " -B \"" << config.build_dir << '"';

    std::string cd_command =
        workpath ? fmt::format("cd {} && ", workpath.value()) : "";

    if (std::system(
            fmt::format("{:s}{:s} {:s}", cd_command, COMMAND, flags.str())
                .c_str()))
        return std::unexpected(
            Error(Error::EXEC_ERROR, "Couldn't initialise CMake project"));

    return {};
}

std::expected<void, Error> handle_init_command(const Command::Flags_t flags) {
    bool override_local = flags & Command::Flags::GLOBAL;
    Config config;
    config.cpp_compiler =
        TRY(haru::Config::get_value("cmake.cpp_compiler", override_local));
    config.c_compiler =
        TRY(haru::Config::get_value("cmake.c_compiler", override_local));
    config.generator =
        TRY(haru::Config::get_value("cmake.generator", override_local));
    config.build_dir =
        TRY(haru::Config::get_value("cmake.build_dir", override_local));

    log::info("CMake settings:\n{}", config);

    TRY(init(config));

    return {};
}
} // namespace cmake
} // namespace haru

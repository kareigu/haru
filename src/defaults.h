#pragma once
#include "project_info.h"
#include <cinttypes>
#include <string_view>
#include <vector>

namespace haru {
constexpr DefaultFiles_t DEFAULT_FILES = DefaultFiles::ALL;
constexpr bool DEFAULT_ADD_DEPENDENCIES = true;
// NOLINTBEGIN(readability-identifier-naming): Would just be constant if
// storing a std::string was contstexpr
constexpr std::vector<Dependency>
DEFAULT_DEPENDENCIES(std::string_view std_version, Language_t languages) {
    if (languages & Language::CPP) {
        if (std::strtoimax(std_version.data(), nullptr, 10) < 23)
            return {Dependency{.name = "fmt",
                               .location = "https://github.com/fmtlib/fmt.git",
                               .version = "10.1.0",
                               .source = Dependency::Source::GIT},
                    Dependency{.name = "Result",
                               .location =
                                   "https://github.com/bitwizeshift/result.git",
                               .version = "master",
                               .source = Dependency::Source::GIT}};
        return {
            Dependency{.name = "fmt",
                       .location = "https://github.com/fmtlib/fmt.git",
                       .version = "10.1.0",
                       .source = Dependency::Source::GIT},
        };
    }
    return {};
}
// NOLINTEND(readability-identifier-naming)

} // namespace haru

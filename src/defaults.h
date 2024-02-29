#pragma once
#include "project_info.h"
#include <array>
#include <cinttypes>
#include <vector>

namespace haru {
constexpr const char* DEFAULT_CMAKE_VERSION = "3.28";
constexpr const char* DEFAULT_VERSION = "0.1.0";
constexpr const char* DEFAULT_LANGUAGES = "cpp";
constexpr std::array<const char*, 2> DEFAULT_STD_VERSIONS{{"23", "17"}};
constexpr const char* DEFAULT_ENTRY_POINT = "src/main";
constexpr DefaultFiles_t DEFAULT_FILES = DefaultFiles::ALL;
constexpr bool DEFAULT_ADD_DEPENDENCIES = true;
// NOLINTNEXTLINE(readability-identifier-naming): Would just be constant if storing a std::string was contstexpr
constexpr std::vector<Dependency> DEFAULT_DEPENDENCIES(const char* std_version) {
  if (std::strtoimax(std_version, nullptr, 10) < 23)
    return {
            Dependency{.name = "fmt", .location = "https://github.com/fmtlib/fmt.git", .version = "10.1.0", .source = Dependency::Source::GIT},
            Dependency{.name = "Result", .location = "https://github.com/bitwizeshift/result.git", .version = "master", .source = Dependency::Source::GIT}};
  return {
          Dependency{.name = "fmt", .location = "https://github.com/fmtlib/fmt.git", .version = "10.1.0", .source = Dependency::Source::GIT},
  };
}

}// namespace haru

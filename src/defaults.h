#pragma once
#include "project_info.h"
#include <array>

namespace haru {
constexpr const char* DEFAULT_CMAKE_VERSION = "3.28";
constexpr const char* DEFAULT_VERSION = "0.1.0";
constexpr const char* DEFAULT_STD_VERSION = "23";
constexpr bool DEFAULT_ADD_DEPENDENCIES = true;

constexpr std::array<Dependency, 2> DEFAULT_DEPENDENCIES() {
  return {
          Dependency{.name = "fmt", .location = "https://github.com/fmtlib/fmt.git", .version = "10.1.0", .source = Dependency::Source::GIT},
          Dependency{.name = "Result", .location = "https://github.com/bitwizeshift/result.git", .version = "master", .source = Dependency::Source::GIT}};
}

}// namespace haru

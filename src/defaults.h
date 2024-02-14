#pragma once
#include "project_info.h"
#include <array>

namespace haru {
constexpr const char* DEFAULT_CMAKE_VERSION = "3.28";
constexpr const char* DEFAULT_VERSION = "0.1.0";
constexpr const char* DEFAULT_LANGUAGES = "cpp";
constexpr std::array<const char*, 2> DEFAULT_STD_VERSIONS() {
  return {
          "23",
          "11"};
};
constexpr const char* DEFAULT_ENTRY_POINT = "src/main";
constexpr bool DEFAULT_ADD_DEPENDENCIES = true;

constexpr std::array<Dependency, 2> DEFAULT_DEPENDENCIES() {
  return {
          Dependency{.name = "fmt", .location = "https://github.com/fmtlib/fmt.git", .version = "10.1.0", .source = Dependency::Source::GIT},
          Dependency{.name = "Result", .location = "https://github.com/bitwizeshift/result.git", .version = "master", .source = Dependency::Source::GIT}};
}

}// namespace haru

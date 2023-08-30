#pragma once
#include "common.h"
#include "error.h"
#include <fmt/core.h>
#include <optional>
#include <result.hpp>
#include <string>
#include <vector>


namespace haru {
struct Dependency {
  enum class Source {
    GIT,
    LOCAL,
  };
  std::string location;
  std::string version;
  Source source;
};


struct ProjectInfo {
  std::string name;
  std::string version;
  std::string standard;
  std::vector<Dependency> dependendencies;

  static cpp::result<ProjectInfo, Error> parse_from_input();

private:
  ProjectInfo() = default;
};
}// namespace haru

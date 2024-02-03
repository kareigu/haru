#pragma once
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
  std::string cmake_version;
  std::string name;
  std::string version;
  std::string standard;
  std::vector<Dependency> dependencies;

  static cpp::result<ProjectInfo, Error> parse_from_input(std::optional<std::string> default_name = {});

private:
  ProjectInfo() = default;
};
}// namespace haru
//
template<>
struct fmt::formatter<haru::Dependency> {
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  auto format(haru::Dependency dependency, format_context& ctx) const {
    const char* source = dependency.source == haru::Dependency::Source::GIT ? "git" : "local";
    std::string output = "";
    output += fmt::format("source = {:s}\n", source);
    output += fmt::format("location = \"{:s}\"\n", dependency.location);
    output += fmt::format("version = {:s}\n", dependency.version);
    return fmt::format_to(ctx.out(), "{{\n {:s}\n}}", output);
  }
};

template<>
struct fmt::formatter<haru::ProjectInfo> {
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  auto format(haru::ProjectInfo info, format_context& ctx) const {
    std::string dependencies = "";
    for (const auto& dependency : info.dependencies) {
      dependencies += fmt::format(" {}\n", dependency);
    }
    if (info.dependencies.size() > 0)
      dependencies = fmt::format("[\n{}\n]", dependencies);

    std::string output = "";
    output += fmt::format("cmake_version = \"{:s}\"\n", info.cmake_version);
    output += fmt::format("name = \"{:s}\"\n", info.name);
    output += fmt::format("version = {:s}\n", info.version);
    output += fmt::format("C++-standard = {:s}\n", info.standard);
    if (info.dependencies.size() > 0)
      output += fmt::format("dependencies = {:s}\n", dependencies);
    return fmt::format_to(ctx.out(), "{:s}", output);
  }
};

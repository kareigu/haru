#pragma once
#include "error.h"
#include <fmt/core.h>
#include <optional>
#include <result.hpp>
#include <sstream>
#include <string>
#include <vector>


namespace haru {
struct Dependency {
  enum class Source {
    GIT,
    LOCAL,
  };
  std::string name;
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
    fmt::format_to(ctx.out(), "{{\n");
    fmt::format_to(ctx.out(), " name = {:s}\n", dependency.name);
    const char* location_name = dependency.source == haru::Dependency::Source::GIT ? "url" : "path";
    fmt::format_to(ctx.out(), " {:s} = \"{:s}\"\n", location_name, dependency.location);
    fmt::format_to(ctx.out(), " version = {:s}\n", dependency.version);
    fmt::format_to(ctx.out(), " source = {:s}\n", source);
    return fmt::format_to(ctx.out(), "}}");
  }
};

template<>
struct fmt::formatter<haru::ProjectInfo> {
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  auto format(haru::ProjectInfo info, format_context& ctx) const {
    std::stringstream dependencies;
    if (info.dependencies.size() > 0) {
      dependencies << "[\n";
      for (const auto& dependency : info.dependencies) {
        std::stringstream ss(fmt::format("{}", dependency));
        std::string line;
        while (std::getline(ss, line, '\n')) {
          dependencies << fmt::format("  {}\n", line);
        }
      }
      dependencies << "]";
    }

    fmt::format_to(ctx.out(), "cmake_version = \"{:s}\"\n", info.cmake_version);
    fmt::format_to(ctx.out(), "name = \"{:s}\"\n", info.name);
    fmt::format_to(ctx.out(), "version = {:s}\n", info.version);
    fmt::format_to(ctx.out(), "C++-standard = {:s}\n", info.standard);
    if (info.dependencies.size() > 0)
      fmt::format_to(ctx.out(), "dependencies = {:s}\n", dependencies.str());
    return ctx.out();
  }
};

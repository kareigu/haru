#pragma once
#include "command.h"
#include "error.h"
#include <array>
#include <cstdint>
#include <expected>
#include <fmt/core.h>
#include <optional>
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

using Language_t = uint8_t;
namespace Language {
  constexpr Language_t NONE = 0;
  constexpr Language_t CPP = 1;
  constexpr Language_t C = 2;
  constexpr Language_t BOTH = CPP | C;


  constexpr const char* to_string(Language_t language) {
    if (language == Language::BOTH)
      return "cpp, c";

    if (language & Language::CPP)
      return "cpp";

    if (language & Language::C)
      return "c";

    return "Unsupported";
  }
}// namespace Language

using DefaultFiles_t = uint8_t;
namespace DefaultFiles {
  constexpr DefaultFiles_t NONE = 0;
  constexpr DefaultFiles_t CLANG_FORMAT = 1;
  constexpr DefaultFiles_t GERSEMIRC = 2;
  constexpr DefaultFiles_t GITIGNORE = 4;
  constexpr DefaultFiles_t ALL = CLANG_FORMAT | GERSEMIRC | GITIGNORE;

  std::string to_string(DefaultFiles_t files);
}// namespace DefaultFiles

struct ProjectInfo {
  std::string cmake_version;
  std::string name;
  std::string version;
  Language_t languages = 0;
  std::array<std::string, 2> standard{};
  std::string entry_point;
  DefaultFiles_t default_files = DefaultFiles::NONE;
  std::vector<Dependency> dependencies;

  static const size_t CPP_INDEX = 0;
  static const size_t C_INDEX = 1;
  static std::expected<ProjectInfo, Error> parse_from_input(Command::Flags_t flags, std::optional<std::string> default_name = {});

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
    fmt::format_to(ctx.out(), "languages = {:s}\n", haru::Language::to_string(info.languages));
    if (info.languages & haru::Language::CPP)
      fmt::format_to(ctx.out(), "C++-standard = {:s}\n", info.standard[0]);
    if (info.languages & haru::Language::C)
      fmt::format_to(ctx.out(), "C-standard = {:s}\n", info.standard[1]);
    fmt::format_to(ctx.out(), "entry_point = {:s}\n", info.entry_point);
    fmt::format_to(ctx.out(), "default_files = {:s}\n", haru::DefaultFiles::to_string(info.default_files));
    if (info.dependencies.size() > 0)
      fmt::format_to(ctx.out(), "dependencies = {:s}\n", dependencies.str());
    return ctx.out();
  }
};

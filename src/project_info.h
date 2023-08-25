#pragma once
#include "common.h"
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

  enum class ErrorType : uint8_t {
    Unknown,
  };
  using ErrorType::Unknown;
  using Error = Error<ErrorType>;

  static cpp::result<ProjectInfo, Error> parse_from_input();

private:
  ProjectInfo() = default;
};
}// namespace haru


template<>
struct fmt::formatter<haru::ProjectInfo::ErrorType> {
  constexpr format_parse_context::iterator parse(format_parse_context& ctx) {
    return ctx.begin(), ctx.end();
  }

  format_context::iterator format(haru::ProjectInfo::ErrorType error_type, format_context& ctx) const {
    using Type = haru::ProjectInfo::ErrorType;
    switch (error_type) {
      case Type::Unknown:
        return format_to(ctx.out(), "UnknownError");
    }
  }
};

template<>
struct fmt::formatter<haru::ProjectInfo::Error> : fmt::formatter<std::string> {
  auto format(haru::ProjectInfo::Error error, format_context& ctx) const -> decltype(ctx.out()) {
    if (error.message) {
      std::string& message = error.message.value();
      return format_to(ctx.out(), "{} - {}", error.type, message);
    } else
      return format_to(ctx.out(), "{}", error.type);
  }
};

#pragma once
#include "fmt/core.h"
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

  struct Error {
    enum class Type : uint8_t {
      Unknown,
    };
    using Type::Unknown;
    Type type;
    std::optional<std::string> message;
  };

  static cpp::result<ProjectInfo, Error> parse_from_input();

private:
  ProjectInfo() = default;
};
}// namespace haru


template<>
struct fmt::formatter<haru::ProjectInfo::Error::Type> {
  constexpr format_parse_context::iterator parse(format_parse_context& ctx) {
    return ctx.begin(), ctx.end();
  }

  format_context::iterator format(haru::ProjectInfo::Error::Type error_type, format_context& ctx) const {
    using Type = haru::ProjectInfo::Error::Type;
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

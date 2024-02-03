#pragma once
#include <cstdint>
#include <fmt/core.h>
#include <limits>
#include <optional>
#include <string>

namespace haru {
class Error {
public:
  using TypeAsInt = uint8_t;
  const static auto TypeMaxValue = std::numeric_limits<TypeAsInt>::max();

  enum class Type : TypeAsInt {
    NoInput,
    Unknown = TypeMaxValue,
  };
  using Type::NoInput;
  using Type::Unknown;

  Error(Type type) : m_type(type) {}
  Error(Type type, std::string message) : m_type(type), m_message(message) {}
  Error() = default;
  ~Error() = default;

  Type type() const { return m_type; }
  const char* type_as_string() const;
  static const char* type_as_string(Type type);
  const std::optional<std::string>& message() const { return m_message; }


private:
  Type m_type = Unknown;
  std::optional<std::string> m_message = {};

  static const char** s_type_as_string;
};
}// namespace haru

template<>
struct fmt::formatter<haru::Error::Type> {
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  auto format(haru::Error::Type error_type, format_context& ctx) const {
    return format_to(ctx.out(), "{}", haru::Error::type_as_string(error_type));
  }
};

template<>
struct fmt::formatter<haru::Error> {
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  auto format(haru::Error error, format_context& ctx) const {
    if (error.message()) {
      const std::string& message = error.message().value();
      return format_to(ctx.out(), "{} - {}", error.type(), message);
    } else
      return format_to(ctx.out(), "{}", error.type());
  }
};

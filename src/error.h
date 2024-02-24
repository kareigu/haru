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
  const static auto TYPE_MAX_VALUE = std::numeric_limits<TypeAsInt>::max();

  enum class Type : TypeAsInt {
    NO_INPUT,
    INPUT_ERROR,
    WRITE_ERROR,
    ALREADY_EXISTS,
    IO_ERROR,
    GENERATE_ERROR,
    EXEC_ERROR,
    NOT_FOUND,
    UNKNOWN_ERROR = TYPE_MAX_VALUE,
  };
  using Type::ALREADY_EXISTS;
  using Type::EXEC_ERROR;
  using Type::GENERATE_ERROR;
  using Type::INPUT_ERROR;
  using Type::IO_ERROR;
  using Type::NO_INPUT;
  using Type::NOT_FOUND;
  using Type::UNKNOWN_ERROR;
  using Type::WRITE_ERROR;

  Error(Type type) : m_type(type) {}
  Error(Type type, std::string message) : m_type(type), m_message(message) {}
  Error() = default;
  ~Error() = default;

  Type type() const { return m_type; }
  const char* type_as_string() const;
  static const char* type_as_string(Type type);
  const std::optional<std::string>& message() const { return m_message; }


private:
  Type m_type = UNKNOWN_ERROR;
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
      return format_to(ctx.out(), "[{}]{}", error.type(), message);
    }
    return format_to(ctx.out(), "[{}]", error.type());
  }
};

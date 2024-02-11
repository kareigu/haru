#pragma once
#include <cstdint>

namespace haru {
class Command {
public:
  enum class Type : uint8_t {
    NoOp,
    Create,
    Init,
  };

  using Flags_t = uint8_t;
  struct Flags {
    static constexpr Command::Flags_t None = 0;
    static constexpr Command::Flags_t UseDefaults = 1;
  };

  using Type::Create;
  using Type::Init;
  using Type::NoOp;


  static Command noop();


public:
  Type type;
  Flags_t flags;
};
}// namespace haru

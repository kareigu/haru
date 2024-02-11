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

  using Type::Create;
  using Type::Init;
  using Type::NoOp;


  static Command noop();
public:
  Type type;
};
}// namespace haru

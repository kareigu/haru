#pragma once
#include <cstdint>

namespace haru {
class Command {
public:
  enum class Type : uint8_t {
    Create,
    Init,
  };

  using Type::Create;
  using Type::Init;
  static Command init();


public:
  Type type;
};
}// namespace haru

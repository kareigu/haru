#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace haru {
class Command {
  public:
    enum class Type : uint8_t {
        NO_OP,
        CREATE,
        INIT,
        CONFIG,
        CMAKE_INIT,
    };

    using Flags_t = uint8_t;
    struct Flags {
        static constexpr Command::Flags_t NONE = 0;
        static constexpr Command::Flags_t USE_DEFAULTS = 1;
        static constexpr Command::Flags_t FORCE = 2;
        static constexpr Command::Flags_t GLOBAL = 4;
        static constexpr Command::Flags_t PATH = 8;
    };

    using Type::CMAKE_INIT;
    using Type::CONFIG;
    using Type::CREATE;
    using Type::INIT;
    using Type::NO_OP;

    static Command noop();

  public:
    Type type;
    Flags_t flags;
    std::vector<std::string> args;
};
} // namespace haru

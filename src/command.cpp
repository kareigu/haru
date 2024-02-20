#include "command.h"

namespace haru {
Command Command::noop() {
  return Command{.type = Type::NO_OP, .flags = Flags::NONE};
}
}// namespace haru

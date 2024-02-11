#include "command.h"

namespace haru {
Command Command::noop() {
  return Command{.type = Type::NoOp, .flags = Flags::None};
}
}// namespace haru

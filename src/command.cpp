#include "command.h"

namespace haru {
Command Command::init() {
  return Command{.type = Type::Init};
}
}// namespace haru

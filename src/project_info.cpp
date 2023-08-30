#include "project_info.h"

namespace haru {
cpp::result<ProjectInfo, Error> ProjectInfo::parse_from_input() {
  return cpp::fail(Error(Error::Unknown));
}

}// namespace haru

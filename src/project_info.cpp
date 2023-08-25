#include "project_info.h"

namespace haru {
cpp::result<ProjectInfo, ProjectInfo::Error> ProjectInfo::parse_from_input() {
  using Error = ProjectInfo::Error;
  return cpp::fail(Error(Unknown));
}

}// namespace haru

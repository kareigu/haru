#include "project_info.h"
#include "utils.h"

namespace haru {
cpp::result<ProjectInfo, Error> ProjectInfo::parse_from_input(std::optional<std::string> default_name) {
  ProjectInfo project_info;
  auto project_name_ret = prompt<std::string>("Project name", default_name);
  if (project_name_ret.has_error())
    return cpp::fail(project_name_ret.error());
  project_info.name = project_name_ret.value();

  auto cmake_version_ret = prompt<std::string>("Minimum cmake version", "3.28");
  if (cmake_version_ret.has_error())
    return cpp::fail(cmake_version_ret.error());
  project_info.cmake_version = cmake_version_ret.value();

  auto version_ret = prompt<std::string>("version", "0.1.0");
  if (version_ret.has_error())
    return cpp::fail(version_ret.error());
  project_info.version = version_ret.value();

  auto standard_ret = prompt<std::string>("C++-standard", "20");
  if (standard_ret.has_error())
    return cpp::fail(standard_ret.error());
  project_info.standard = standard_ret.value();


  return project_info;
}

}// namespace haru

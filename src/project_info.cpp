#include "project_info.h"
#include "defaults.h"
#include "utils.h"
#include <sstream>

namespace haru {
cpp::result<void, Error> handle_adding_dependencies(ProjectInfo& project_info);

cpp::result<ProjectInfo, Error> ProjectInfo::parse_from_input(std::optional<std::string> default_name) {
  ProjectInfo project_info;
  auto project_name_ret = prompt<std::string>("Project name", default_name);
  if (project_name_ret.has_error())
    return cpp::fail(project_name_ret.error());
  project_info.name = project_name_ret.value();

  auto cmake_version_ret = prompt<std::string>("Minimum cmake version", DEFAULT_CMAKE_VERSION);
  if (cmake_version_ret.has_error())
    return cpp::fail(cmake_version_ret.error());
  project_info.cmake_version = cmake_version_ret.value();

  auto version_ret = prompt<std::string>("version", DEFAULT_VERSION);
  if (version_ret.has_error())
    return cpp::fail(version_ret.error());
  project_info.version = version_ret.value();

  auto standard_ret = prompt<std::string>("C++-standard", DEFAULT_STD_VERSION);
  if (standard_ret.has_error())
    return cpp::fail(standard_ret.error());
  project_info.standard = standard_ret.value();

  auto add_dependencies_ret = prompt_yes_no("Add dependencies?", DEFAULT_ADD_DEPENDENCIES);
  if (add_dependencies_ret.has_error())
    return cpp::fail(add_dependencies_ret.error());

  if (add_dependencies_ret.value()) {
    auto added_dependencies_ret = handle_adding_dependencies(project_info);
    if (added_dependencies_ret.has_error())
      return cpp::fail(added_dependencies_ret.error());
  }

  return project_info;
}

cpp::result<void, Error> handle_adding_dependencies(ProjectInfo& project_info) {
  auto default_dependencies = DEFAULT_DEPENDENCIES();
  std::stringstream defaults_formatted;
  size_t last_index = default_dependencies.size() - 1;
  for (size_t i = 0; i < last_index; i++)
    defaults_formatted << fmt::format("{}, ", default_dependencies[i].name);
  if (last_index >= 0)
    defaults_formatted << default_dependencies[last_index].name;


  auto add_defaults_ret = prompt_yes_no(
          fmt::format("Add default dependencies? [{}]", defaults_formatted.str()).c_str(),
          DEFAULT_ADD_DEPENDENCIES);
  if (add_defaults_ret.has_error())
    return cpp::fail(add_defaults_ret.error());

  if (add_defaults_ret.value()) {
    spdlog::info("Adding defaul dependencies");
    project_info.dependencies = std::vector<Dependency>(default_dependencies.begin(), default_dependencies.end());
  }

  return {};
}

}// namespace haru

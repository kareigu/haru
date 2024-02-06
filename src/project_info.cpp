#include "project_info.h"
#include "defaults.h"
#include "utils.h"
#include <sstream>

namespace haru {
cpp::result<void, Error> handle_adding_dependencies(ProjectInfo& project_info);

cpp::result<ProjectInfo, Error> ProjectInfo::parse_from_input(std::optional<std::string> default_name) {
  ProjectInfo project_info;
  std::string project_name = TRY(prompt<std::string>("Project name", default_name));
  project_info.name = project_name;

  std::string cmake_version = TRY(prompt<std::string>("Minimum cmake version", DEFAULT_CMAKE_VERSION));
  project_info.cmake_version = cmake_version;

  std::string version = TRY(prompt<std::string>("version", DEFAULT_VERSION));
  project_info.version = version;

  std::string standard = TRY(prompt<std::string>("C++-standard", DEFAULT_STD_VERSION));
  project_info.standard = standard;

  bool add_dependencies = TRY(prompt_yes_no("Add dependencies?", DEFAULT_ADD_DEPENDENCIES));
  if (add_dependencies) {
    TRY(handle_adding_dependencies(project_info));
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


  auto add_defaults = TRY(prompt_yes_no(
          fmt::format("Add default dependencies? [{}]", defaults_formatted.str()).c_str(),
          DEFAULT_ADD_DEPENDENCIES));

  if (add_defaults) {
    spdlog::info("Adding defaul dependencies");
    project_info.dependencies = std::vector<Dependency>(default_dependencies.begin(), default_dependencies.end());
  }

  return {};
}

}// namespace haru

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

  while (true) {
    bool add_dependency = TRY(prompt_yes_no("Add more dependencies?", !add_defaults));
    if (!add_dependency)
      return {};

    Dependency dependency;
    std::string source_string = TRY(prompt<std::string>("source [git, local]", "git"));
    Dependency::Source source = Dependency::Source::GIT;
    if (source_string == "git" || source_string == "GIT" || source_string == "Git")
      source = Dependency::Source::GIT;
    else if (source_string == "local" || source_string == "LOCAL" || source_string == "Local")
      source = Dependency::Source::LOCAL;
    else
      return cpp::fail(Error(Error::InputError, "must be git or local"));

    dependency.source = source;

    switch (source) {
      case Dependency::Source::GIT: {
        std::string name = TRY(prompt<std::string>("name"));
        dependency.name = name;
        std::string location = TRY(prompt<std::string>("URL"));
        dependency.location = location;
        std::string version = TRY(prompt<std::string>("version or tag", "master"));
        dependency.version = version;
        break;
      }
      case Dependency::Source::LOCAL: {
        std::string name = TRY(prompt<std::string>("name"));
        dependency.name = name;
        std::string location = TRY(prompt<std::string>("path"));
        dependency.location = location;
        dependency.version = "LOCAL";
        break;
      }
    }

    project_info.dependencies.push_back(dependency);
  }

  return {};
}

}// namespace haru

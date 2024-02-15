#include "project_info.h"
#include "defaults.h"
#include "utils.h"
#include <sstream>

namespace haru {
cpp::result<void, Error> handle_adding_dependencies(ProjectInfo& project_info);

cpp::result<ProjectInfo, Error> ProjectInfo::parse_from_input(Command::Flags_t flags, std::optional<std::string> default_name) {
  ProjectInfo project_info;
  auto default_std_versions = DEFAULT_STD_VERSIONS();
  if (flags & Command::Flags::UseDefaults) {
    auto default_dependencies = DEFAULT_DEPENDENCIES();
    if (default_name.has_value())
      project_info.name = default_name.value();
    else
      project_info.name = TRY(prompt<std::string>("Project name"));
    project_info.cmake_version = DEFAULT_CMAKE_VERSION;
    project_info.version = DEFAULT_VERSION;
    project_info.languages = Language::cpp;
    project_info.standard[0] = default_std_versions[0];
    project_info.standard[1] = default_std_versions[1];
    project_info.entry_point = fmt::format("{:s}.cpp", DEFAULT_ENTRY_POINT);
    project_info.dependencies = std::vector<Dependency>(default_dependencies.begin(), default_dependencies.end());
    return project_info;
  }
  project_info.name = TRY(prompt<std::string>("Project name", default_name));
  project_info.cmake_version = TRY(prompt<std::string>("Minimum CMake version", DEFAULT_CMAKE_VERSION));
  project_info.version = TRY(prompt<std::string>("Version", DEFAULT_VERSION));

  std::vector<std::string> input_languages = TRY(prompt_list<std::string>(
          "Languages",
          std::vector<std::string>{Language::to_string(Language::cpp), Language::to_string(Language::c)},
          std::vector<std::string>{Language::to_string(Language::cpp)}));
  for (const auto& language : input_languages) {
    if (language == Language::to_string(Language::cpp)) {
      project_info.languages |= Language::cpp;
      std::string standard = TRY(prompt<std::string>("C++-standard", default_std_versions[0]));
      project_info.standard[0] = standard;
    }
    if (language == Language::to_string(Language::c)) {
      project_info.languages |= Language::c;
      std::string standard = TRY(prompt<std::string>("C-standard", default_std_versions[1]));
      project_info.standard[1] = standard;
    }
  }
  if (project_info.languages == Language::none)
    return cpp::fail(Error(Error::InputError, "You need select at least 1 valid language"));

  std::string default_entry_point = project_info.languages & Language::cpp ? fmt::format("{:s}.cpp", DEFAULT_ENTRY_POINT) : fmt::format("{:s}.c", DEFAULT_ENTRY_POINT);
  project_info.entry_point = TRY(prompt<std::string>("Entrypoint", default_entry_point));

  bool add_dependencies = TRY(prompt_yes_no("Add dependencies?", DEFAULT_ADD_DEPENDENCIES));
  if (add_dependencies) {
    TRY(handle_adding_dependencies(project_info));
  }

  spdlog::info("\nCurrent settings:\n{}", project_info);
  bool proceed = TRY(prompt_yes_no("Proceed?", true));
  if (!proceed)
    return ProjectInfo::parse_from_input(flags, default_name);

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
        std::string location = TRY(prompt<std::string>("url"));
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
        dependency.version = "local";
        break;
      }
    }

    project_info.dependencies.push_back(dependency);
  }

  return {};
}

}// namespace haru

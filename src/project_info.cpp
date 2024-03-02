#include "project_info.h"
#include "command.h"
#include "defaults.h"
#include "error.h"
#include "log.h"
#include "utils.h"
#include <cstddef>
#include <expected>
#include <fmt/core.h>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace haru {
std::expected<void, Error> handle_adding_dependencies(ProjectInfo& project_info, bool use_defaults);

std::expected<ProjectInfo, Error> ProjectInfo::parse_from_input(Command::Flags_t flags, std::optional<std::string> default_name) {
  ProjectInfo project_info;
  bool use_defaults = flags & Command::Flags::USE_DEFAULTS;

  project_info.name = use_defaults && default_name.has_value()
                            ? default_name.value()
                            : TRY(prompt<std::string>("Project name", default_name));
  project_info.cmake_version = use_defaults
                                     ? DEFAULT_CMAKE_VERSION
                                     : TRY(prompt<std::string>("Minimum CMake version", DEFAULT_CMAKE_VERSION));
  project_info.version = use_defaults
                               ? DEFAULT_VERSION
                               : TRY(prompt<std::string>("Version", DEFAULT_VERSION));

  std::vector<std::string> default_languages{DEFAULT_LANGUAGES};
  std::vector<std::string> input_languages = use_defaults
                                                   ? default_languages
                                                   : TRY(prompt_list<std::string>(
                                                           "Languages",
                                                           std::vector<std::string>{
                                                                   Language::to_string(Language::CPP),
                                                                   Language::to_string(Language::C)},
                                                           default_languages));
  for (const auto& language : input_languages) {
    if (language == Language::to_string(Language::CPP)) {
      project_info.languages |= Language::CPP;
      std::string default_standard = DEFAULT_STD_VERSIONS[ProjectInfo::CPP_INDEX];
      std::string standard = use_defaults
                                   ? default_standard
                                   : TRY(prompt<std::string>("C++-standard", default_standard));
      project_info.standard[ProjectInfo::CPP_INDEX] = standard;
    }
    if (language == Language::to_string(Language::C)) {
      project_info.languages |= Language::C;
      std::string default_standard = DEFAULT_STD_VERSIONS[ProjectInfo::C_INDEX];
      std::string standard = use_defaults
                                   ? default_standard
                                   : TRY(prompt<std::string>("C-standard", default_standard));
      project_info.standard[ProjectInfo::C_INDEX] = standard;
    }
  }
  if (project_info.languages == Language::NONE)
    return std::unexpected(Error(Error::INPUT_ERROR, "You need select at least 1 valid language"));

  std::string default_entry_point = project_info.languages & Language::CPP
                                          ? fmt::format("{:s}.cpp", DEFAULT_ENTRY_POINT)
                                          : fmt::format("{:s}.c", DEFAULT_ENTRY_POINT);
  project_info.entry_point = use_defaults ? default_entry_point : TRY(prompt<std::string>("Entrypoint", default_entry_point));

  std::vector<std::string> default_files = {
          DefaultFiles::to_string(DefaultFiles::CLANG_FORMAT),
          DefaultFiles::to_string(DefaultFiles::GERSEMIRC),
          DefaultFiles::to_string(DefaultFiles::GITIGNORE),
  };
  std::vector<std::string> input_default_files = use_defaults
                                                       ? std::move(default_files)
                                                       : TRY(prompt_list<std::string>("Default files", default_files, default_files));
  for (const auto& file : input_default_files) {
    if (file == DefaultFiles::to_string(DefaultFiles::CLANG_FORMAT)) {
      project_info.default_files |= DefaultFiles::CLANG_FORMAT;
      continue;
    }
    if (file == DefaultFiles::to_string(DefaultFiles::GERSEMIRC)) {
      project_info.default_files |= DefaultFiles::GERSEMIRC;
      continue;
    }
    if (file == DefaultFiles::to_string(DefaultFiles::GITIGNORE)) {
      project_info.default_files |= DefaultFiles::GITIGNORE;
      continue;
    }
  }

  bool add_dependencies = use_defaults ? DEFAULT_ADD_DEPENDENCIES : TRY(prompt_yes_no("Add dependencies?", DEFAULT_ADD_DEPENDENCIES));
  if (add_dependencies) {
    TRY(handle_adding_dependencies(project_info, use_defaults));
  }

  log::info("\nCurrent settings:\n{}", project_info);
  bool proceed = TRY(prompt_yes_no("Proceed?", true));
  if (!proceed)
    return ProjectInfo::parse_from_input(flags, default_name);

  return project_info;
}

std::expected<void, Error> handle_adding_dependencies(ProjectInfo& project_info, bool use_defaults) {
  auto default_dependencies = DEFAULT_DEPENDENCIES(project_info.standard[ProjectInfo::CPP_INDEX].c_str());
  std::stringstream defaults_formatted;
  size_t last_index = default_dependencies.size() - 1;
  for (size_t i = 0; i < last_index; i++)
    defaults_formatted << fmt::format("{}, ", default_dependencies[i].name);
  if (last_index >= 0)
    defaults_formatted << default_dependencies[last_index].name;


  bool add_defaults = use_defaults
                            ? DEFAULT_ADD_DEPENDENCIES
                            : TRY(prompt_yes_no(
                                    fmt::format(
                                            "Add default dependencies? [{}]",
                                            defaults_formatted.str())
                                            .c_str(),
                                    DEFAULT_ADD_DEPENDENCIES));

  if (add_defaults) {
    log::info("Adding default dependencies");
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
      return std::unexpected(Error(Error::INPUT_ERROR, "must be git or local"));

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

std::string DefaultFiles::to_string(DefaultFiles_t files) {
  if (files == NONE)
    return "none";
  std::ostringstream ss;
  if (files & CLANG_FORMAT)
    ss << "clang_format, ";

  if (files & GERSEMIRC)
    ss << "gersemirc, ";

  if (files & GITIGNORE)
    ss << "gitignore, ";

  std::string str = ss.str();
  if (str.size() == 0)
    return "Unsupported";

  str.pop_back();
  str.pop_back();

  return str;
}
}// namespace haru

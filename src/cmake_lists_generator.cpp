#include "cmake_lists_generator.h"
#include "error.h"
#include "project_info.h"
#include <expected>
#include <fmt/core.h>
#include <sstream>
#include <string>

namespace haru {
CMakeListsGenerator::CMakeListsGenerator(const ProjectInfo& project_info) : m_project_info(project_info) {}


std::expected<std::string, Error> CMakeListsGenerator::generate() {
  std::stringstream output;
  bool include_fetch = false;
  if (m_project_info.dependencies.size() > 0) {
    for (const auto& dependency : m_project_info.dependencies) {
      if (dependency.source == Dependency::Source::GIT) {
        include_fetch = true;
        break;
      }
    }
  }
  output << fmt::format("cmake_minimum_required(VERSION {:s})\n", m_project_info.cmake_version);

  if (include_fetch)
    output << "\ninclude(FetchContent)\n";

  output << fmt::format("\nproject(\n  {:s}\n  VERSION {:s}\n  LANGUAGES ", m_project_info.name, m_project_info.version);
  if (m_project_info.languages & Language::CPP)
    output << "CXX ";
  if (m_project_info.languages & Language::C)
    output << "C";
  output << "\n)\n";

  if (include_fetch) {
    output << "set(FETCHCONTENT_QUIET FALSE)\n";

    for (const auto& dependency : m_project_info.dependencies) {
      if (dependency.source == Dependency::Source::GIT)
        output << fmt::format("\nfetchcontent_declare(\n  {:s}\n  GIT_REPOSITORY {:s}\n  GIT_TAG {:s}\n  USES_TERMINAL_DOWNLOAD TRUE\n)\nfetchcontent_makeavailable({:s})\n",
                              dependency.name, dependency.location, dependency.version, dependency.name);
      else
        output << "\n#Unimplemented(LOCAL dependency)\n";
    }
  }

  output << fmt::format("\nset(PROJECT_SOURCES {:s})\n", m_project_info.entry_point);

  output << "\nadd_executable(${PROJECT_NAME} ${PROJECT_SOURCES})\n";
  if (m_project_info.languages & Language::CPP) {
    output << "\nset_target_properties(${PROJECT_NAME} PROPERTIES";
    output << fmt::format("\n  CXX_STANDARD {:s}", m_project_info.standard[0]);
    output << "\n  CXX_STANDARD_REQUIRED TRUE\n)\n";
  }
  if (m_project_info.languages & Language::C) {
    output << "\nset_target_properties(${PROJECT_NAME} PROPERTIES";
    output << fmt::format("\n  C_STANDARD {:s}", m_project_info.standard[1]);
    output << "\n  C_STANDARD_REQUIRED TRUE\n)\n";
  }
  output << "target_link_libraries(\n  ${PROJECT_NAME}\n";
  if (include_fetch) {
    for (const auto& dependency : m_project_info.dependencies)
      output << fmt::format("  {:s}\n", dependency.name);
  }
  output << ")\n";

  if (output.fail() || output.bad())
    return std::unexpected(Error(Error::GENERATE_ERROR, "Error writing generated CMakesLists.txt to buffer"));


  return output.str();
}
}// namespace haru

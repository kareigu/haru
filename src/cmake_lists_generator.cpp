#include "cmake_lists_generator.h"
#include <fmt/core.h>
#include <sstream>

namespace haru {
CMakeListsGenerator::CMakeListsGenerator(const ProjectInfo& project_info) : m_project_info(project_info) {}


cpp::result<std::string, Error> CMakeListsGenerator::generate() {
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

  output << fmt::format("\nproject(\n  {:s}\n  VERSION {:s}\n  LANGUAGES CXX\n)", m_project_info.name, m_project_info.version);

  output << fmt::format("\nset(CMAKE_CXX_STANDARD {:s})\n", m_project_info.standard);
  output << "set(CMAKE_CXX_STANDARD_REQUIRED TRUE)\n";


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

  output << "\nset(PROJECT_SOURCES src/main.cpp)\n";

  output << "\nadd_executable(${PROJECT_NAME} ${PROJECT_SOURCES})\n";
  output << "target_link_libraries(\n  ${PROJECT_NAME}\n";
  if (include_fetch) {
    for (const auto& dependency : m_project_info.dependencies)
      output << fmt::format("  {:s}\n", dependency.name);
  }
  output << ")\n";


  return output.str();
}
}// namespace haru

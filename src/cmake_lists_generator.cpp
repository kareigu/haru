#include "cmake_lists_generator.h"

namespace haru {
CMakeListsGenerator::CMakeListsGenerator(const ProjectInfo& project_info) : m_project_info(project_info) {}


cpp::result<std::string, Error> CMakeListsGenerator::generate() {
  return cpp::fail(Error(Error::Type::Unknown));
}
}// namespace haru

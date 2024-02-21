#pragma once
#include "error.h"
#include "project_info.h"
#include <expected>
#include <string>

namespace haru {
class CMakeListsGenerator {
public:
  CMakeListsGenerator(const ProjectInfo& project_info);
  ~CMakeListsGenerator() = default;
  CMakeListsGenerator() = delete;

  std::expected<std::string, Error> generate();

private:
  ProjectInfo m_project_info;
};
}// namespace haru

#pragma once
#include "error.h"
#include "project_info.h"
#include <result.hpp>
#include <string>

namespace haru {
class CMakeListsGenerator {
public:
  CMakeListsGenerator(const ProjectInfo& project_info);
  ~CMakeListsGenerator() = default;
  CMakeListsGenerator() = delete;

  cpp::result<std::string, Error> generate();

private:
  ProjectInfo m_project_info;
};
}// namespace haru

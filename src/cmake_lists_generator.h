#pragma once
#include "error.h"
#include "project_info.h"
#include <expected>
#include <filesystem>
#include <string>
#include <vector>

namespace haru {
class CMakeListsGenerator {
public:
  struct CMakeFile {
    std::filesystem::path filepath;
    std::string contents;
  };
  CMakeListsGenerator(const ProjectInfo& project_info);
  ~CMakeListsGenerator() = default;
  CMakeListsGenerator() = delete;

  std::expected<std::vector<CMakeFile>, Error> generate();

private:
  std::expected<std::string, Error> generate_cmakelists() const;
  std::expected<std::string, Error> generate_dependencies() const;
  std::expected<std::string, Error> generate_compile_flags() const;
  std::expected<std::string, Error> generate_tidy() const;

  void write_templated_string(std::stringstream& output, const std::string_view placeholder_name) const;
  bool should_generate_dependencies() const;
  ProjectInfo m_project_info;
};
}// namespace haru

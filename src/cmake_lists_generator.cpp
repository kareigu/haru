#include "cmake_lists_generator.h"
#include "bake_in.h"
#include "error.h"
#include "log.h"
#include "project_info.h"
#include "utils.h"
#include <cstddef>
#include <expected>
#include <fmt/core.h>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace haru {
CMakeListsGenerator::CMakeListsGenerator(const ProjectInfo& project_info) : m_project_info(project_info) {}

constexpr const char TEMPLATE_CHARACTER = '@';
constexpr const char* FETCH_DEPENDENCIES_FN = "fetch_project_dependencies()";
constexpr const char* LINK_DEPENDENCIES_FN = "link_project_dependencies(${PROJECT_NAME})";
constexpr const char* ADD_TIDY_FN = "add_clang_tidy(${PROJECT_NAME} ${CMAKE_CURRENT_BINARY_DIR})";

#define INCLUDE_STR(FILE) "include(${CMAKE_SOURCE_DIR}/cmake/" FILE ")"

std::optional<std::string_view> gather_templated_string(const std::string_view templ, size_t& i) {
  if (templ[i] == TEMPLATE_CHARACTER && templ[i + 1] == TEMPLATE_CHARACTER) {
    i += 2;
    size_t start = i;
    while (templ[i] != TEMPLATE_CHARACTER) {
      i++;
    }
    size_t end = i;
    i += 2;
    return std::string_view(&templ[start], &templ[end]);
  }
  return std::nullopt;
}

void CMakeListsGenerator::write_templated_string(std::stringstream& output, const std::string_view placeholder_name) const {
  if (placeholder_name == "CMAKE_VERSION") {
    output << m_project_info.cmake_version;
    return;
  }
  if (placeholder_name == "DEPENDENCIES_INCLUDE") {
    if (should_generate_dependencies())
      output << INCLUDE_STR("dependencies.cmake");
    return;
  }
  if (placeholder_name == "TIDY_INCLUDE") {
    output << INCLUDE_STR("tidy.cmake");
    return;
  }
  if (placeholder_name == "FETCH_CONTENT_INCLUDE") {
    if (should_generate_dependencies())
      output << "include(FetchContent)";
    return;
  }
  if (placeholder_name == "PROJECT_NAME") {
    output << m_project_info.name;
    return;
  }
  if (placeholder_name == "PROJECT_VERSION") {
    output << m_project_info.version;
    return;
  }
  if (placeholder_name == "LANGUAGES") {
    if (m_project_info.languages == Language::BOTH) {
      output << "CXX C";
      return;
    }
    if (m_project_info.languages & Language::CPP) {
      output << "CXX";
      return;
    }
    if (m_project_info.languages & Language::C) {
      output << "C";
      return;
    }
    log::warn("No languages defined");
  }
  if (placeholder_name == "FETCH_DEPENDENCIES") {
    if (should_generate_dependencies())
      output << FETCH_DEPENDENCIES_FN;
    return;
  }
  if (placeholder_name == "ENTRYPOINT") {
    output << m_project_info.entry_point;
    return;
  }
  if (placeholder_name == "CXX_STANDARD") {
    if (m_project_info.languages & Language::CPP)
      output << "CXX_STANDARD " << m_project_info.standard[ProjectInfo::CPP_INDEX];
    return;
  }
  if (placeholder_name == "C_STANDARD") {
    if (m_project_info.languages & Language::C)
      output << "C_STANDARD " << m_project_info.standard[ProjectInfo::C_INDEX];
    return;
  }
  if (placeholder_name == "LINK_DEPENDENCIES") {
    if (m_project_info.dependencies.empty())
      return;
    output << LINK_DEPENDENCIES_FN;
    return;
  }
  if (placeholder_name == "START_DEPENDENCIES") {
    for (const auto& dependency : m_project_info.dependencies) {
      if (dependency.source == Dependency::Source::LOCAL) {
        log::warn("LOCAL dependencies are unimplemented");
        continue;
      }
      output << "    fetchcontent_declare(\n";
      output << "        " << dependency.name << '\n';
      output << "        GIT_REPOSITORY " << dependency.location << '\n';
      output << "        GIT_TAG " << dependency.version << '\n';
      output << "        GIT_PROGRESS TRUE\n";
      output << "        USES_TERMINAL_DOWNLOAD TRUE\n";
      output << "        SYSTEM\n";
      output << "    )\n";
      output << "    fetchcontent_makeavailable(" << dependency.name << ")\n\n";
    }
    return;
  }
  if (placeholder_name == "START_DEPENDENCIES_LINK") {
    for (const auto& dependency : m_project_info.dependencies)
      output << "        " << dependency.name << '\n';
    return;
  }
  if (placeholder_name == "ADD_TIDY") {
    output << ADD_TIDY_FN;
    return;
  }
  if (placeholder_name == "TIDY_SOURCES") {
    auto entry_point = std::filesystem::path(m_project_info.entry_point);
    std::string source_directory("");
    if (entry_point.has_parent_path())
      source_directory = entry_point.parent_path().string() + "/";

    if (m_project_info.languages & Language::CPP) {
      output << "            " << source_directory << "*.cpp\n";
      output << "            " << source_directory << "*.hpp\n";
    }
    if (m_project_info.languages & Language::C) {
      output << "            " << source_directory << "*.c\n";
    }
    output << "            " << source_directory << "*.h\n";
    return;
  }

  log::warn("Unsupported templating parameter {}", placeholder_name);
}

std::expected<std::string, Error> CMakeListsGenerator::generate_cmakelists() const {
  std::stringstream output;

  std::string_view cmake_template(reinterpret_cast<char*>(&bake_in_cmakelists), reinterpret_cast<char*>(&bake_in_cmakelists[bake_in_cmakelists_len]));
  for (size_t i = 0; i < cmake_template.size(); i++) {
    auto template_opt = gather_templated_string(cmake_template, i);
    if (template_opt.has_value())
      write_templated_string(output, template_opt.value());

    output << cmake_template[i];
  }

  if (output.fail() || output.bad())
    return std::unexpected(Error(Error::GENERATE_ERROR, "Error writing generated CMakesLists.txt to buffer"));


  return output.str();
}


std::expected<std::string, Error> CMakeListsGenerator::generate_dependencies() const {
  std::stringstream output;

  std::string_view cmake_template(reinterpret_cast<char*>(&bake_in_cmake_dependencies),
                                  reinterpret_cast<char*>(&bake_in_cmake_dependencies[bake_in_cmake_dependencies_len]));
  for (size_t i = 0; i < cmake_template.size(); i++) {
    auto template_opt = gather_templated_string(cmake_template, i);
    if (template_opt.has_value())
      write_templated_string(output, template_opt.value());

    output << cmake_template[i];
  }

  if (output.fail() || output.bad())
    return std::unexpected(Error(Error::GENERATE_ERROR, "Error writing generated compile_flags.cmake to buffer"));


  return output.str();
}

std::expected<std::string, Error> CMakeListsGenerator::generate_compile_flags() const {
  std::stringstream output;
  for (unsigned int i = 0; i < bake_in_cmake_compile_flags_len; i++)
    output << bake_in_cmake_compile_flags[i];

  if (output.fail() || output.bad())
    return std::unexpected(Error(Error::GENERATE_ERROR, "Error writing generated compile_flags.cmake to buffer"));


  return output.str();
}

std::expected<std::string, Error> CMakeListsGenerator::generate_tidy() const {
  std::stringstream output;
  std::string_view cmake_template(reinterpret_cast<char*>(&bake_in_cmake_tidy),
                                  reinterpret_cast<char*>(&bake_in_cmake_tidy[bake_in_cmake_tidy_len]));
  for (size_t i = 0; i < cmake_template.size(); i++) {
    auto template_opt = gather_templated_string(cmake_template, i);
    if (template_opt.has_value())
      write_templated_string(output, template_opt.value());

    output << cmake_template[i];
  }

  if (output.fail() || output.bad())
    return std::unexpected(Error(Error::GENERATE_ERROR, "Error writing generated tidy.cmake to buffer"));


  return output.str();
}


std::expected<std::vector<CMakeListsGenerator::CMakeFile>, Error> CMakeListsGenerator::generate() {
  std::vector<CMakeFile> files;
  files.reserve(4);

  std::string cmakelists_contents = TRY(generate_cmakelists());
  files.emplace_back(CMakeFile{.filepath = "CMakeLists.txt", .contents = std::move(cmakelists_contents)});

  if (should_generate_dependencies()) {
    std::string dependencies_contents = TRY(generate_dependencies());
    files.emplace_back(CMakeFile{.filepath = "cmake/dependencies.cmake", .contents = std::move(dependencies_contents)});
  }

  std::string compile_flags_contents = TRY(generate_compile_flags());
  files.emplace_back(CMakeFile{.filepath = "cmake/compile_flags.cmake", .contents = std::move(compile_flags_contents)});

  if (m_project_info.default_files & DefaultFiles::CLANG_FORMAT) {
    std::string tidy_contents = TRY(generate_tidy());
    files.emplace_back(CMakeFile{.filepath = "cmake/tidy.cmake", .contents = std::move(tidy_contents)});
  }

  return files;
}

bool CMakeListsGenerator::should_generate_dependencies() const {
  return !m_project_info.dependencies.empty();
}
}// namespace haru

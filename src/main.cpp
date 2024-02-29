#include "args_parser.h"
#include "cmake_lists_generator.h"
#include "command.h"
#include "error.h"
#include "file_operations.h"
#include "log.h"
#include "project_info.h"
#include "utils.h"
#include <cstdlib>
#include <filesystem>
#include <fmt/core.h>
#include <optional>
#include <string>


int main(int argc, char** argv) {
#ifndef NDEBUG
  haru::log::set_level(haru::log::Level::DEBUG);
#endif
  haru::arg_parse::init();

  if (argc <= 1) {
    haru::log::error(haru::Error(haru::Error::INPUT_ERROR, "No arguments provided"));
    haru::log::info("{:s}", haru::arg_parse::help_string());
    return EXIT_FAILURE;
  }

  auto parse_ret = haru::arg_parse::parse(argc, argv);
  if (!parse_ret) {
    haru::log::error(parse_ret.error());
    haru::log::info("{:s}", haru::arg_parse::help_string());
    return EXIT_FAILURE;
  }

  auto ran_command = parse_ret.value();


  if (ran_command.type == haru::Command::CREATE || ran_command.type == haru::Command::INIT) {
    bool init = ran_command.type == haru::Command::INIT;
    std::optional<std::string> default_name = {};
    if (init) {
      default_name = std::filesystem::current_path().filename().string();
    }
    auto project_info = MUST(haru::ProjectInfo::parse_from_input(ran_command.flags, default_name));

    haru::CMakeListsGenerator cmake_generator(project_info);
    auto cmake_files = MUST(cmake_generator.generate());

    std::filesystem::path workpath = MUST(haru::file_ops::create_work_directory(init, project_info.name, ran_command.flags & haru::Command::Flags::FORCE));
    MUST(haru::file_ops::write_cmake_files(workpath, cmake_files));
    MUST(haru::file_ops::write_entry_point(workpath, project_info.entry_point, project_info.languages));
    MUST(haru::file_ops::write_default_files(workpath, project_info.default_files));

    haru::log::info("Generation complete for {}", workpath);

    haru::file_ops::FormatGeneratedFilesArgs format_generated_files_args{
            .default_files = project_info.default_files,
            .files = cmake_files,
            .source_path = project_info.entry_point};
    WARN(haru::file_ops::format_generated_files(workpath, format_generated_files_args));
  }

  return EXIT_SUCCESS;
}

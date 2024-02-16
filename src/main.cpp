#include "args_parser.h"
#include "cmake_lists_generator.h"
#include "command.h"
#include "file_operations.h"
#include "log_formatter.h"
#include "project_info.h"
#include "utils.h"
#include <cstdlib>
#include <filesystem>
#include <fmt/core.h>
#include <memory>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>


int main(int argc, char** argv) {
#ifndef NDEBUG
  spdlog::set_level(spdlog::level::debug);
#endif
  auto logger = spdlog::dual_color_mt("log");
  auto formatter = std::make_unique<haru::LogFormatter>();
  spdlog::set_default_logger(logger);
  spdlog::set_formatter(formatter->clone());

  if (argc <= 1) {
    spdlog::error("No arguments given");
    spdlog::info("{:s}", haru::ArgsParser::help_string());
    return EXIT_FAILURE;
  }

  auto parse_ret = haru::ArgsParser::parse(argc, argv);
  if (parse_ret.has_error()) {
    spdlog::error("{}", parse_ret.error());
    spdlog::info("{:s}", haru::ArgsParser::help_string());
    return EXIT_FAILURE;
  }

  auto ran_command = parse_ret.value();


  if (ran_command.type == haru::Command::Create || ran_command.type == haru::Command::Init) {
    bool init = ran_command.type == haru::Command::Init;
    std::optional<std::string> default_name = {};
    if (init) {
      default_name = std::filesystem::current_path().filename();
    }
    auto project_info = MUST(haru::ProjectInfo::parse_from_input(ran_command.flags, default_name));

    haru::CMakeListsGenerator cmake_generator(project_info);
    auto generate_ret = cmake_generator.generate();
    if (generate_ret.has_error()) {
      spdlog::error("Couldn't generate CMakeLists.txt: {}", generate_ret.error());
      return EXIT_FAILURE;
    }
    std::string cmake_lists_contents = generate_ret.value();

    auto workpath_ret = MUST(haru::create_work_directory(init, project_info.name));
    std::filesystem::path workpath = workpath_ret;

    auto cmake_lists_write_ret = haru::write_cmake_lists(workpath, cmake_lists_contents);
    if (cmake_lists_write_ret.has_error()) {
      spdlog::error("{}", cmake_lists_write_ret.error());
      return EXIT_FAILURE;
    }

    MUST(haru::write_entry_point(workpath, project_info.entry_point, project_info.languages));
    MUST(haru::write_default_files(workpath, project_info.default_files));
  }

  return EXIT_SUCCESS;
}

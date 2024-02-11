#include "args_parser.h"
#include "cmake_lists_generator.h"
#include "command.h"
#include "log_formatter.h"
#include "project_info.h"
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
    std::optional<std::string> default_name = {};
    if (ran_command.type == haru::Command::Init) {
      default_name = std::filesystem::current_path().filename();
    }
    auto project_info_ret = haru::ProjectInfo::parse_from_input(ran_command.flags, default_name);
    if (project_info_ret.has_error()) {
      spdlog::error("Couldn't parse project info: {}", project_info_ret.error());
      return EXIT_FAILURE;
    }
    auto project_info = project_info_ret.value();
    spdlog::info("{}", project_info);

    haru::CMakeListsGenerator cmake_generator(project_info);
    auto generate_ret = cmake_generator.generate();
    if (generate_ret.has_error()) {
      spdlog::error("Couldn't generate CMakeLists.txt: {}", generate_ret.error());
      return EXIT_FAILURE;
    }
    std::string cmake_lists_contents = generate_ret.value();
  }

  return EXIT_SUCCESS;
}

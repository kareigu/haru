#include "args_parser.h"
#include "log_formatter.h"
#include "project_info.h"
#include <cstdlib>
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
    spdlog::info("{:s}", haru::ArgsParser::usage_string());
    return EXIT_FAILURE;
  }

  auto ran_command = parse_ret.value();


  if (ran_command == haru::ArgsParser::Command::Create) {
    auto project_info_ret = haru::ProjectInfo::parse_from_input();
    if (project_info_ret.has_error()) {
      spdlog::error("Couldn't parse project info: {}", project_info_ret.error());
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}

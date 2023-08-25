#include "common.h"
#include "log_formatter.h"
#include "project_info.h"
#include <argparse/argparse.hpp>
#include <cstdlib>
#include <fmt/core.h>
#include <iostream>
#include <memory>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <stdexcept>


int main(int argc, char** argv) {
#ifndef NDEBUG
  spdlog::set_level(spdlog::level::debug);
#endif
  auto logger = spdlog::dual_color_mt("log");
  auto formatter = std::make_unique<haru::LogFormatter>();
  spdlog::set_default_logger(logger);
  spdlog::set_formatter(formatter->clone());
  argparse::ArgumentParser arg_parser(HARU_PRG_NAME, HARU_VERSION);
  arg_parser.add_description("C++ cmake project generator");

  argparse::ArgumentParser create_command("create");
  create_command.add_description("Create a project in a new directory");

  argparse::ArgumentParser init_command("init");
  init_command.add_description("Initialise a project in the current directory");

  arg_parser.add_subparser(create_command);
  arg_parser.add_subparser(init_command);

  if (argc <= 1) {
    spdlog::info("{:s}", arg_parser.help().str());
    return EXIT_FAILURE;
  }

  try {
    arg_parser.parse_args(argc, argv);
  } catch (std::runtime_error e) {
    spdlog::error("{:s}", e.what());
    spdlog::info("{:s}", arg_parser.usage());
    return EXIT_FAILURE;
  }

  if (arg_parser.is_subcommand_used(create_command)) {
    auto project_info_ret = haru::ProjectInfo::parse_from_input();
    if (project_info_ret.has_error()) {
      spdlog::error("Couldn't parse project info: {}", project_info_ret.error());
      return EXIT_FAILURE;
    }
  }


  return EXIT_SUCCESS;
}

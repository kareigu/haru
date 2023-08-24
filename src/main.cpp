#include "common.h"
#include "log_formatter.h"
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

  return EXIT_SUCCESS;
}

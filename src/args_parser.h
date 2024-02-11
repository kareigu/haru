#pragma once
#include "command.h"
#include "error.h"
#include <args.hxx>
#include <result.hpp>

namespace haru {
class ArgsParser {
public:
  ArgsParser() = delete;
  ~ArgsParser() = default;


  static cpp::result<Command, Error> parse(int argc, char** argv);

  static const auto help_string() { return s_parser.Help(); }
  static const auto usage_string() { return s_parser.Help(); }

private:
  static args::Group s_commands_group;

  static std::array<args::Command, 2> s_commands;
  static std::array<args::Flag, 3> s_flags;
  static std::array<args::HelpFlag, 3> s_help_flags;

  static args::ArgumentParser s_parser;
};
}// namespace haru

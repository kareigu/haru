#pragma once
#include "common.h"
#include "error.h"
#include <args.hxx>
#include <map>
#include <memory>
#include <result.hpp>

namespace haru {
class ArgsParser {
public:
  ArgsParser() = delete;
  ~ArgsParser() = default;

  enum class Command : uint8_t {
    Create,
    Init,
  };

  static cpp::result<Command, Error> parse(int argc, char** argv);

  static const auto help_string() { return s_parser.Help(); }
  static const auto usage_string() { return s_parser.Help(); }

private:
  static args::Group s_commands_group;
  static args::HelpFlag s_help_flag;

  static std::array<args::Command, 2> s_commands;
  static std::array<args::Flag, 1> s_flags;

  static args::ArgumentParser s_parser;
};
}// namespace haru

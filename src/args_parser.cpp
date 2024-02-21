#include "args_parser.h"
#include "command.h"
#include "common.h"
#include "error.h"
#include <argparse/argparse.hpp>
#include <exception>
#include <expected>
#include <string>

constexpr const char* EPILOG = HARU_PRG_NAME " v" HARU_VERSION " - " RELEASE_TYPE;

namespace haru {

namespace arg_parse {
  static argparse::ArgumentParser s_prg(HARU_PRG_NAME, HARU_VERSION);
  static argparse::ArgumentParser s_create_cmd("create", HARU_VERSION);
  static argparse::ArgumentParser s_init_cmd("init", HARU_VERSION);

  void init() {
    s_prg.add_description("Generate CMake projects for C/C++");
    s_prg.add_epilog(EPILOG);

    s_create_cmd.add_description("Create a project in a new directory");
    s_create_cmd.add_argument("-d", "--use-defaults").flag().help("Use defaults for all possible fields");
    s_create_cmd.add_argument("-f", "--force").flag();

    s_init_cmd.add_description("Initialise a project in the current directory");
    s_init_cmd.add_argument("-d", "--use-defaults").flag().help("Use defaults for all fields");

    s_prg.add_subparser(s_create_cmd);
    s_prg.add_subparser(s_init_cmd);
  }


  std::expected<Command, Error> parse(int argc, char** argv) {
    try {
      s_prg.parse_args(argc, argv);
    } catch (const std::exception& e) {
      return std::unexpected(Error(Error::UNKNOWN_ERROR, e.what()));
    }

    Command::Flags_t flags = Command::Flags::NONE;
    if (s_prg.is_subcommand_used(s_create_cmd)) {
      if (s_create_cmd.get<bool>("-d"))
        flags |= Command::Flags::USE_DEFAULTS;
      if (s_create_cmd.get<bool>("-f"))
        flags |= Command::Flags::FORCE;
      return Command{.type = Command::CREATE, .flags = flags};
    }

    if (s_prg.is_subcommand_used(s_init_cmd)) {
      if (s_init_cmd.get<bool>("-d"))
        flags |= Command::Flags::USE_DEFAULTS;
      return Command{.type = Command::INIT, .flags = flags};
    }


    return std::unexpected(Error(Error::UNKNOWN_ERROR));
  }

  std::string help_string() {
    return s_prg.help().str();
  }

  std::string usage_string() {
    return help_string();
  }
}// namespace arg_parse
}// namespace haru

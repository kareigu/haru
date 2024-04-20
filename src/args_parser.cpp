#include "args_parser.h"
#include "command.h"
#include "common.h"
#include "error.h"
#include <argparse/argparse.hpp>
#include <expected>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

constexpr const char* EPILOG = HARU_PRG_NAME " v" HARU_VERSION " - " RELEASE_TYPE;

namespace haru {

namespace arg_parse {
  static argparse::ArgumentParser s_prg(HARU_PRG_NAME, HARU_VERSION);
  static argparse::ArgumentParser s_create_cmd("create", HARU_VERSION);
  static argparse::ArgumentParser s_init_cmd("init", HARU_VERSION);
  static argparse::ArgumentParser s_config_cmd("config", HARU_VERSION);
  static argparse::ArgumentParser s_cmake_cmd("cmake", HARU_VERSION);
  static argparse::ArgumentParser s_cmake_init_cmd("init", HARU_VERSION);

  void init() {
    s_prg.add_description("Generate CMake projects for C/C++");
    s_prg.add_epilog(EPILOG);

    s_create_cmd.add_description("Create a project in a new directory");
    s_create_cmd.add_argument("-d", "--use-defaults").flag().help("Use defaults for all possible fields");
    s_create_cmd.add_argument("-f", "--force").flag();

    s_init_cmd.add_description("Initialise a project in the current directory");
    s_init_cmd.add_argument("-d", "--use-defaults").flag().help("Use defaults for all fields");
    s_init_cmd.add_argument("-f", "--force").flag();

    s_config_cmd.add_description("Access configuration options");
    s_config_cmd
            .add_argument("key")
            .nargs(0, 1)
            .help("Key of configuration value to access");
    s_config_cmd
            .add_argument("value")
            .remaining()
            .nargs(0, 1)
            .help("Value to set");
    s_config_cmd.add_argument("-g", "--global")
            .flag()
            .help("Operate on global configuration instead of local");
    s_config_cmd.add_argument("-p", "--path")
            .flag()
            .help("Print out the paths to currently effective config files");

    s_cmake_cmd.add_description("Run CMake related commands");
    s_cmake_init_cmd.add_description("Initialise CMake using a available config");
    s_cmake_init_cmd
            .add_argument("-g", "--global")
            .flag()
            .help("Force using the global config instead of the local one");
    s_cmake_cmd.add_subparser(s_cmake_init_cmd);


    s_prg.add_subparser(s_create_cmd);
    s_prg.add_subparser(s_init_cmd);
    s_prg.add_subparser(s_config_cmd);
    s_prg.add_subparser(s_cmake_cmd);
  }


  std::expected<Command, Error> parse(int argc, char** argv) {
    try {
      s_prg.parse_args(argc, argv);
    } catch (const std::runtime_error& e) {
      return std::unexpected(Error(Error::UNKNOWN_ERROR, e.what()));
    }

    Command::Flags_t flags = Command::Flags::NONE;
    if (s_prg.is_subcommand_used(s_create_cmd)) {
      if (s_create_cmd.get<bool>("-d"))
        flags |= Command::Flags::USE_DEFAULTS;
      if (s_create_cmd.get<bool>("-f"))
        flags |= Command::Flags::FORCE;
      return Command{.type = Command::CREATE, .flags = flags, .args = {}};
    }

    if (s_prg.is_subcommand_used(s_init_cmd)) {
      if (s_init_cmd.get<bool>("-d"))
        flags |= Command::Flags::USE_DEFAULTS;
      if (s_init_cmd.get<bool>("-f"))
        flags |= Command::Flags::FORCE;
      return Command{.type = Command::INIT, .flags = flags, .args = {}};
    }

    if (s_prg.is_subcommand_used(s_config_cmd)) {
      if (s_config_cmd.get<bool>("-g"))
        flags |= Command::Flags::GLOBAL;
      if (s_config_cmd.get<bool>("-p"))
        flags |= Command::Flags::PATH;
      std::vector<std::string> args;
      try {
        args.push_back(s_config_cmd.get("key"));
        args.push_back(s_config_cmd.get("value"));
      } catch (const std::logic_error& e) {}
      if (args.empty() && !(flags & Command::Flags::PATH))
        return std::unexpected(Error(Error::NO_INPUT, "No key/value provided"));
      return Command{.type = Command::CONFIG, .flags = flags, .args = args};
    }

    if (s_prg.is_subcommand_used(s_cmake_cmd)) {
      if (s_cmake_cmd.is_subcommand_used(s_cmake_init_cmd)) {
        if (s_cmake_init_cmd.get<bool>("-g"))
          flags |= Command::Flags::GLOBAL;

        return Command{.type = Command::CMAKE_INIT, .flags = flags, .args = {}};
      }

      return std::unexpected(Error(Error::INPUT_ERROR, "Subcommand needs to be provided"));
    }


    return std::unexpected(Error(Error::UNKNOWN_ERROR, "Unhandled command provided"));
  }

  std::string help_string() {
    if (s_prg.is_subcommand_used(s_create_cmd))
      return s_create_cmd.help().str();
    if (s_prg.is_subcommand_used(s_init_cmd))
      return s_init_cmd.help().str();
    if (s_prg.is_subcommand_used(s_config_cmd))
      return s_config_cmd.help().str();
    if (s_cmake_cmd.is_subcommand_used(s_cmake_init_cmd))
      return s_cmake_init_cmd.help().str();
    if (s_prg.is_subcommand_used(s_cmake_cmd))
      return s_cmake_cmd.help().str();
    return s_prg.help().str();
  }

  std::string usage_string() {
    return help_string();
  }
}// namespace arg_parse
}// namespace haru

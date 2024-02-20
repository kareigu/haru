#include "args_parser.h"
#include "command.h"
#include "common.h"
#include "error.h"
#include "log.h"
#include <args.hxx>
#include <array>
#include <cstddef>
#include <result.hpp>


namespace haru {
constexpr const char* EPILOG = HARU_PRG_NAME " v" HARU_VERSION " - " RELEASE_TYPE;
args::ArgumentParser ArgsParser::s_parser = args::ArgumentParser("C++ cmake project generator", EPILOG);
args::Group ArgsParser::s_commands_group = args::Group(s_parser, "COMMANDS", args::Group::Validators::Xor, args::Options::Global);

std::array<args::Command, 2> ArgsParser::s_commands = {
        args::Command(ArgsParser::s_commands_group, "create", "Create a project in a new directory"),
        args::Command(ArgsParser::s_commands_group, "init", "Initialise a project in the current directory"),
};
enum CommandIndex : size_t {
  CREATE,
  INIT,
};

std::array<args::Flag, 4> ArgsParser::s_flags = {
        args::Flag(ArgsParser::s_parser, "version", "version", {'v', "version"}),
        args::Flag(ArgsParser::s_commands[CommandIndex::CREATE], "use-defaults", "Use default values", {'d', "use-defaults"}),
        args::Flag(ArgsParser::s_commands[CommandIndex::INIT], "use-defaults", "Use default values", {'d', "use-defaults"}),
        args::Flag(ArgsParser::s_commands[CommandIndex::CREATE], "force", "Force new directory creation", {'f', "force"}),
};

std::array<args::HelpFlag, 3> ArgsParser::s_help_flags = {
        args::HelpFlag(ArgsParser::s_parser, "help", "help", {'h', "help"}),
        args::HelpFlag(ArgsParser::s_commands[CommandIndex::CREATE], "help", "help", {'h', "help"}),
        args::HelpFlag(ArgsParser::s_commands[CommandIndex::INIT], "help", "help", {'h', "help"}),
};


cpp::result<Command, Error> ArgsParser::parse(int argc, char** argv) {
  try {
    s_parser.ParseCLI(argc, argv);
  } catch (const args::Help& e) {
    log::info("{}", help_string());
    return Command::noop();
  } catch (const args::Error& e) {
    if (s_flags[0]) {
      log::info(HARU_VERSION);
      return Command::noop();
    }
    return cpp::fail(Error(Error::UNKNOWN_ERROR, e.what()));
  }

  if (s_flags[0]) {
    log::info(HARU_VERSION);
    return Command::noop();
  }

  if (s_commands[CommandIndex::CREATE]) {
    Command::Flags_t flags = Command::Flags::NONE;
    if (s_flags[1])
      flags |= Command::Flags::USE_DEFAULTS;
    if (s_flags[3])
      flags |= Command::Flags::FORCE;
    return Command{.type = Command::CREATE, .flags = flags};
  }

  if (s_commands[CommandIndex::INIT]) {
    Command::Flags_t flags = Command::Flags::NONE;
    if (s_flags[2])
      flags |= Command::Flags::USE_DEFAULTS;
    return Command{.type = Command::INIT, .flags = flags};
  }

  return cpp::fail(Error(Error::UNKNOWN_ERROR));
}
}// namespace haru

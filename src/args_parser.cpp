#include "args_parser.h"
#include "args.hxx"
#include "common.h"
#include <cstddef>
#include <spdlog/spdlog.h>


namespace haru {
constexpr const char* EPILOG = HARU_PRG_NAME " v" HARU_VERSION " - " RELEASE_TYPE;
args::ArgumentParser ArgsParser::s_parser = args::ArgumentParser("C++ cmake project generator", EPILOG);
args::Group ArgsParser::s_commands_group = args::Group(s_parser, "COMMANDS", args::Group::Validators::Xor, args::Options::Global);

std::array<args::Command, 2> ArgsParser::s_commands = {
        args::Command(ArgsParser::s_commands_group, "create", "Create a project in a new directory"),
        args::Command(ArgsParser::s_commands_group, "init", "Initialise a project in the current directory"),
};
enum CommandIndex : size_t {
  Create,
  Init,
};

std::array<args::Flag, 4> ArgsParser::s_flags = {
        args::Flag(ArgsParser::s_parser, "version", "version", {'v', "version"}),
        args::Flag(ArgsParser::s_commands[CommandIndex::Create], "use-defaults", "Use default values", {'d', "use-defaults"}),
        args::Flag(ArgsParser::s_commands[CommandIndex::Init], "use-defaults", "Use default values", {'d', "use-defaults"}),
        args::Flag(ArgsParser::s_commands[CommandIndex::Create], "force", "Force new directory creation", {'f', "force"}),
};

std::array<args::HelpFlag, 3> ArgsParser::s_help_flags = {
        args::HelpFlag(ArgsParser::s_parser, "help", "help", {'h', "help"}),
        args::HelpFlag(ArgsParser::s_commands[CommandIndex::Create], "help", "help", {'h', "help"}),
        args::HelpFlag(ArgsParser::s_commands[CommandIndex::Init], "help", "help", {'h', "help"}),
};


cpp::result<Command, Error> ArgsParser::parse(int argc, char** argv) {
  try {
    s_parser.ParseCLI(argc, argv);
  } catch (const args::Help& e) {
    spdlog::info("{}", help_string());
    return Command::noop();
  } catch (const args::Error& e) {
    if (s_flags[0]) {
      spdlog::info(HARU_VERSION);
      return Command::noop();
    }
    return cpp::fail(Error(Error::Unknown, e.what()));
  }

  if (s_flags[0]) {
    spdlog::info(HARU_VERSION);
    return Command::noop();
  }

  if (s_commands[CommandIndex::Create]) {
    Command::Flags_t flags = Command::Flags::None;
    if (s_flags[1])
      flags |= Command::Flags::UseDefaults;
    if (s_flags[3])
      flags |= Command::Flags::Force;
    return Command{.type = Command::Create, .flags = flags};
  }

  if (s_commands[CommandIndex::Init]) {
    Command::Flags_t flags = Command::Flags::None;
    if (s_flags[2])
      flags |= Command::Flags::UseDefaults;
    return Command{.type = Command::Init, .flags = flags};
  }

  return cpp::fail(Error(Error::Unknown));
}
}// namespace haru

#include "args_parser.h"
#include "args.hxx"
#include <spdlog/spdlog.h>


namespace haru {
constexpr const char* EPILOG = HARU_PRG_NAME " v" HARU_VERSION;
args::ArgumentParser ArgsParser::s_parser = args::ArgumentParser("C++ cmake project generator", EPILOG);
args::Group ArgsParser::s_commands_group = args::Group(s_parser, "COMMANDS");

std::array<args::Command, 2> ArgsParser::s_commands = {
        args::Command(ArgsParser::s_commands_group, "create", "Create a project in a new directory"),
        args::Command(ArgsParser::s_commands_group, "init", "Initialise a project in the current directory"),
};

std::array<args::Flag, 1> ArgsParser::s_flags = {
        args::Flag(ArgsParser::s_parser, "version", "version", {'v', "version"}),
};

args::HelpFlag ArgsParser::s_help_flag = args::HelpFlag(ArgsParser::s_parser, "help", "help", {'h', "help"});


cpp::result<ArgsParser::Command, Error> ArgsParser::parse(int argc, char** argv) {
  try {
    s_parser.ParseCLI(argc, argv);
  } catch (const args::Help& e) {
    spdlog::info("{}", help_string());
    return Command::Init;
  } catch (const args::Error& e) {
    if (s_flags[0]) {
      spdlog::info(HARU_VERSION);
      return Command::Init;
    }
    return cpp::fail(Error(Error::Unknown, e.what()));
  }

  if (s_flags[0]) {
    spdlog::info(HARU_VERSION);
    return Command::Init;
  }


  return cpp::fail(Error(Error::Unknown));
}
}// namespace haru

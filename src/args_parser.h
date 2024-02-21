#pragma once
#include "command.h"
#include "error.h"
#include <expected>
#include <string>

namespace haru {
namespace arg_parse {

  void init();
  std::expected<Command, Error> parse(int argc, char** argv);

  [[nodiscard("Creates a new std::string")]] std::string help_string();
  [[nodiscard("Creates a new std::string")]] std::string usage_string();

};// namespace arg_parse
}// namespace haru

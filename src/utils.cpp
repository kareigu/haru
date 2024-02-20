#include "utils.h"
#include "error.h"
#include <fmt/core.h>
#include <iostream>
#include <ranges>
#include <result.hpp>
#include <string>

namespace haru {
cpp::result<bool, Error> prompt_yes_no(const char* text, bool default_value, bool new_line) {
  std::string default_formatted = fmt::format("({:c}/{:c})", default_value ? 'Y' : 'y', default_value ? 'n' : 'N');
  fmt::print("{:s} {:s}: ", text, default_formatted);
  if (new_line)
    fmt::println("");

  std::string value_input;
  std::getline(std::cin, value_input);

  auto is_space = [](char c) { return c == ' '; };
  value_input = std::ranges::to<std::string>(value_input | std::views::drop_while(is_space) | std::views::reverse | std::views::drop_while(is_space) | std::views::reverse);

  if (value_input.empty())
    return default_value;

  if (value_input == "Y" || value_input == "y" || value_input == "yes" || value_input == "YES" || value_input == "Yes")
    return true;

  if (value_input == "N" || value_input == "n" || value_input == "no" || value_input == "NO" || value_input == "No")
    return false;

  return cpp::fail(Error(Error::INPUT_ERROR, "Only y/n/<empty> are allowed"));
}
}// namespace haru

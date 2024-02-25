#include "utils.h"
#include "error.h"
#include <algorithm>
#include <cctype>
#include <expected>
#include <fmt/core.h>
#include <iostream>
#include <ranges>
#include <string>
#include <string_view>

namespace haru {
std::expected<bool, Error> prompt_yes_no(const char* text, bool default_value, bool new_line) {
  std::string default_formatted = fmt::format("({:c}/{:c})", default_value ? 'Y' : 'y', default_value ? 'n' : 'N');
  fmt::print("{:s} {:s}: ", text, default_formatted);
  if (new_line)
    fmt::println("");

  std::string value_input;
  std::getline(std::cin, value_input);

  auto is_space = [](char c) { return c == ' '; };
  value_input = std::ranges::to<std::string>(value_input | std::views::drop_while(is_space) | std::views::reverse | std::views::drop_while(is_space) | std::views::reverse);
  std::transform(value_input.begin(), value_input.end(), value_input.begin(), [](char c) { return std::tolower(c); });

  if (value_input.empty())
    return default_value;

  if (value_input == "y" || value_input == "yes")
    return true;

  if (value_input == "n" || value_input == "no")
    return false;

  return std::unexpected(Error(Error::INPUT_ERROR, "Only y/n/<empty> are allowed"));
}

std::expected<void, Error> check_command_exists(const std::string_view command, const std::string_view flags) {
#if _WIN32
  if (std::system(fmt::format("{:s} {:s} > /dev/null 2>&1", command, flags).c_str()))
    return std::unexpected(Error(Error::NOT_FOUND, fmt::format("{:s} does not exist in path", command)));
#else
  if (std::system(fmt::format("{:s} {:s} > /dev/null 2>&1", command, flags).c_str()))
    return std::unexpected(Error(Error::NOT_FOUND, fmt::format("{:s} does not exist in path", command)));
#endif
  return {};
}
}// namespace haru

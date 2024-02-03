#pragma once
#include "error.h"
#include <fmt/core.h>
#include <iostream>
#include <optional>
#include <result.hpp>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>

namespace haru {

/**
 * @brief Prompt the user for input and return the obtained value as T
 * @param text - text to print as the prompt
 * @param default_value - default value if nothing is entered. If empty, Error will be returned
 * @param new_line - whether to print a new line after prompt text
 * @return T on success, on error Error with more information
 */
template<typename T>
cpp::result<T, Error> prompt(const char* text, std::optional<T> default_value = {}, bool new_line = false) {
  auto default_value_formatted = default_value.has_value() ? fmt::format(" ({})", default_value.value()) : "";
  fmt::print("{:s}{:s}: ", text, default_value_formatted, new_line);
  if (new_line)
    fmt::println("");

  std::string value_input;
  std::getline(std::cin, value_input);
  std::istringstream ss(value_input);
  if (!default_value.has_value() && value_input.empty())
    return cpp::fail(Error(Error::Type::NoInput, fmt::format("{} needs to be given", text)));

  if (default_value.has_value() && value_input.empty())
    return default_value.value();
  T value;
  ss >> value;
  return value;
}

}// namespace haru

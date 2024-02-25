#pragma once
#include "error.h"
#include <cstdlib>
#include <expected>
#include <optional>
#include <vector>
// NOLINTBEGIN(misc-include-cleaner): Wrong
#include <fmt/core.h>
#include <iostream>
#include <sstream>
#include <string>
// NOLINTEND(misc-include-cleaner)

#define TRY(EXPR) ({                           \
  auto&& _temp_ret = (EXPR);                   \
  if (!_temp_ret)                              \
    return std::unexpected(_temp_ret.error()); \
  _temp_ret.value();                           \
})

#define MUST(EXPR) ({                    \
  auto&& _temp_ret = (EXPR);             \
  if (!_temp_ret) {                      \
    haru::log::error(_temp_ret.error()); \
    std::exit(EXIT_FAILURE);             \
  }                                      \
  _temp_ret.value();                     \
})

#define WARN(EXPR) ({                   \
  auto&& _temp_ret = (EXPR);            \
  if (!_temp_ret) {                     \
    haru::log::warn(_temp_ret.error()); \
    std::exit(EXIT_FAILURE);            \
  }                                     \
})

namespace haru {

/**
 * @brief Prompt the user for input and return the obtained value as T
 * @param text - text to print as the prompt
 * @param default_value - default value if nothing is entered. If empty, Error will be returned
 * @param new_line - whether to print a new line after prompt text
 * @return T on success, on error Error with more information
 */
template<typename T>
std::expected<T, Error> prompt(const char* text, std::optional<T> default_value = {}, bool new_line = false) {
  auto default_value_formatted = default_value.has_value() ? fmt::format(" ({})", default_value.value()) : "";
  fmt::print("{:s}{:s}: ", text, default_value_formatted, new_line);
  if (new_line)
    fmt::println("");

  std::string value_input;
  std::getline(std::cin, value_input);
  std::istringstream ss(value_input);
  if (!default_value.has_value() && value_input.empty())
    return std::unexpected(Error(Error::Type::NO_INPUT, fmt::format("{} needs to be given", text)));

  if (default_value.has_value() && value_input.empty())
    return default_value.value();
  T value;
  ss >> value;
  return value;
}

template<typename T>
std::expected<std::vector<T>, Error> prompt_list(const char* text, std::vector<T> valid_values, std::optional<std::vector<T>> default_values = {}, bool new_line = false) {
  std::string default_values_formatted;
  if (default_values.has_value()) {
    auto values = default_values.value();
    if (values.empty())
      goto empty_defaults;
    std::stringstream ss;
    ss << " (";
    for (size_t i = 0; i < values.size() - 1; i++)
      ss << fmt::format("{}, ", values[i]);
    ss << fmt::format("{})", values[values.size() - 1]);
    default_values_formatted = ss.str();
  }
empty_defaults:

  std::string valid_values_formatted;
  if (valid_values.empty())
    return std::unexpected(Error(Error::UNKNOWN_ERROR, "Implementation error, missing valid values"));

  {
    std::ostringstream ss;
    ss << "[";
    for (size_t i = 0; i < valid_values.size() - 1; i++)
      ss << fmt::format("{}, ", valid_values[i]);
    ss << fmt::format("{}]", valid_values[valid_values.size() - 1]);
    valid_values_formatted = ss.str();
  }

  fmt::print("{:s} {:s}{:s}: ", text, valid_values_formatted, default_values_formatted);
  if (new_line)
    fmt::println("");

  std::string value_input;
  std::getline(std::cin, value_input);


  if (!default_values.has_value() && value_input.empty())
    return std::unexpected(Error(Error::Type::NO_INPUT, fmt::format("{} needs to be given", text)));

  if (default_values.has_value() && value_input.empty())
    return default_values.value();

  std::vector<std::istringstream> inputs;
  size_t prev_start = 0;
  for (size_t i = 0; i < value_input.size(); i++) {
    const char& c = value_input[i];
    if (c != ' ' && c != ',')
      continue;
    if (i == 0)
      continue;
    if (value_input[i - 1] == ' ' || value_input[i - 1] == ',')
      continue;


    inputs.push_back(std::istringstream(std::string(value_input.data() + prev_start, value_input.data() + i)));
    prev_start = i + 1;
  }
  inputs.push_back(std::istringstream(std::string(value_input.data() + prev_start, value_input.data() + value_input.size())));

  std::vector<T> ret(inputs.size());
  for (size_t i = 0; i < inputs.size(); i++) {
    auto& input = inputs[i];
    T value;
    input >> value;
    if (input.bad() || input.fail())
      return std::unexpected(Error(Error::INPUT_ERROR, fmt::format("Invalid value provided: {:s}, valid values: {:s}", input.str(), valid_values_formatted)));

    bool invalid = true;
    for (const auto& valid_value : valid_values) {
      if (valid_value == value) {
        invalid = false;
        break;
      }
    }
    if (invalid)
      return std::unexpected(Error(Error::INPUT_ERROR, fmt::format("Invalid value provided: {:s}, valid values: {:s}", input.str(), valid_values_formatted)));


    ret[i] = std::move(value);
  }

  return ret;
}

std::expected<bool, Error> prompt_yes_no(const char* text, bool default_value, bool new_line = false);

std::expected<void, Error> check_command_exists(const std::string_view command, const std::string_view flags = "--version");

}// namespace haru

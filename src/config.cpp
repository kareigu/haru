#include "config.h"
#include "command.h"
#include "error.h"
#include "log.h"
#include "utils.h"
#include <expected>
#include <filesystem>
#include <fmt/core.h>
#include <fmt/std.h>
#include <fstream>
#include <pwd.h>
#include <stdlib.h>
#include <string_view>
#include <unistd.h>
#include <vector>

namespace haru {
const std::filesystem::path Config::get_global_config_path() {
  const char* home = "";
  if ((home = getenv("XDG_CONFIG_HOME")) == nullptr) {
    if ((home = getenv("HOME")) == nullptr) {
      home = getpwuid(getuid())->pw_dir;
    }
    auto path = std::filesystem::absolute(fmt::format("{:s}/.config/{:s}", home, CONFIG_FILE_NAME));
    return path;
  }
  auto path = std::filesystem::absolute(fmt::format("{:s}/{:s}", home, CONFIG_FILE_NAME));
  return path;
}

std::optional<const std::filesystem::path> Config::get_local_config_path() {
  if (std::filesystem::exists(CONFIG_FILE_NAME))
    return CONFIG_FILE_NAME;

  std::filesystem::path current_path = std::filesystem::current_path();
  while (current_path != "/") {
    auto possible_config = fmt::format("{}/{:s}", current_path, CONFIG_FILE_NAME);
    if (std::filesystem::exists(possible_config))
      return possible_config;

    if (!current_path.has_parent_path())
      return std::nullopt;
    current_path = current_path.parent_path();
  }
  return std::nullopt;
}

std::expected<Config::Config_t, Error> Config::get_global_config() {
  auto path = get_global_config_path();
  if (!std::filesystem::exists(path))
    return default_config();
  return get_config(path);
}
std::expected<Config::Config_t, Error> Config::get_local_config() {
  auto path = get_local_config_path();
  if (!path)
    return default_config();
  return get_config(path.value());
}

std::expected<Config::Config_t, Error> Config::get_config(const std::filesystem::path& path) {
  std::ifstream file(path);

  Config_t config;

  std::string current_header;
  size_t current_line = 0;
  while (true) {
    std::string line;
    std::getline(file, line);
    current_line++;

    if (line.starts_with('[')) {
      long end = static_cast<long>(line.find(']', 1));
      current_header = std::string(line.begin() + 1, line.begin() + end);
      continue;
    }

    if (line.contains('=')) {
      long split_point = static_cast<long>(line.find('='));
      std::string_view local_key(line.begin(), line.begin() + split_point);
      std::string_view value(line.begin() + split_point + 1, line.end());

      std::string key = current_header.empty()
                              ? std::string(local_key)
                              : fmt::format("{:s}.{:s}", current_header, local_key);

      config[key] = std::string(value);
      continue;
    }

    if (file.eof()) {
      break;
    }

    if (line.empty() || line == "\n" || line == "\n\r")
      continue;


    if (file.fail())
      return std::unexpected(Error(
              Error::IO_ERROR,
              fmt::format("Failed while reading config file at line {:d}: {:s}", current_line, line)));

    return std::unexpected(Error(
            Error::CONFIG_ERROR,
            fmt::format("Invalid configuration value at line {:d}: '{:s}'", current_line, line)));
  }

  return config;
}

std::expected<void, Error> Config::write_value(const std::string& key, const std::string& value, bool global) {
  std::filesystem::path filepath;
  if (global) {
    filepath = get_global_config_path();
  } else {
    auto local = get_local_config_path();
    if (!local)
      return std::unexpected(Error(Error::NOT_FOUND, "Create a config file inside current project first"));
    filepath = local.value();
  }

  std::ifstream read_file(filepath);
  std::stringstream output;

  std::string_view input_header;
  std::string_view real_key;
  if (!key.contains('.')) {
    real_key = std::string_view(key);
  } else {
    long divider = static_cast<long>(key.find('.'));
    input_header = std::string_view(key.begin(), key.begin() + divider);
    real_key = std::string_view(key.begin() + divider + 1, key.end());
  }

  bool under_correct_header = input_header.empty();
  bool wrote = false;
  while (true) {
    std::string line;
    std::getline(read_file, line);

    if (wrote) {
      goto writing_dump_current_line;
    }

    if (line.starts_with('[')) {
      if (!wrote && under_correct_header) {
        log::debug("writing {:s}", real_key);
        output << fmt::format("{:s}={:s}\n", real_key, value);
        wrote = true;
      }
      long end = static_cast<long>(line.find(']', 1));
      std::string_view line_header(line.begin() + 1, line.begin() + end);
      if (line_header == input_header) {
        log::debug("Found existing header: {:s}", line_header);
        under_correct_header = true;
      } else {
        under_correct_header = false;
      }
    } else if (line.starts_with(fmt::format("{:s}=", real_key))) {
      if (!input_header.empty()) {
        if (under_correct_header) {
          output << fmt::format("{:s}={:s}\n", real_key, value);
          wrote = true;
          continue;
        }
      } else {
        output << fmt::format("{:s}={:s}\n", real_key, value);
        wrote = true;
        continue;
      }
    }

  writing_dump_current_line:
    output << line;
    if (read_file.eof())
      break;
    output << '\n';

    if (read_file.fail())
      return std::unexpected(Error(
              Error::IO_ERROR,
              fmt::format("Failed while reading config file")));
  }
  read_file.close();

  if (!wrote && !input_header.empty()) {
    if (!under_correct_header)
      output << fmt::format("[{:s}]\n", input_header);
    output << fmt::format("{:s}={:s}\n", real_key, value);
  }

  std::ofstream write_file(filepath);
  write_file << output.rdbuf();
  if (write_file.fail())
    return std::unexpected(Error(Error::IO_ERROR, "Failed while writing config file"));
  write_file.close();
  return {};
}

std::expected<void, Error> Config::handle_config_command(const std::vector<std::string>& args, const Command::Flags_t flags) {
  if (flags & Command::Flags::PATH) {
    auto global_config = Config::get_global_config_path();
    auto local_config = get_local_config_path();
    if (!std::filesystem::exists(global_config) && !local_config)
      log::info("No configuration files found");
    if (std::filesystem::exists(global_config))
      log::info("Global config path: {}", global_config);
    if (local_config)
      log::info("Local config path: {}", local_config.value());
    return {};
  }

  bool global = flags & Command::Flags::GLOBAL;
  Config_t config;
  if (global)
    config = TRY(get_global_config());
  else
    config = TRY(get_local_config());

  if (args.empty())
    return std::unexpected(Error(Error::NO_INPUT, "No key/value provided"));

  const std::string& key = args[0];
  if (args.size() == 1) {
    log::info("{:s}", config[key]);
    return {};
  }
  const std::string& value = args[1];

  const std::string PREV_VALUE = config[key];
  if (PREV_VALUE.empty())
    log::info("Setting {:s} to {:s}", key, value);
  else
    log::info("Setting {:s} from {:s} to {:s}", key, PREV_VALUE, value);
  TRY(write_value(key, value, global));
  return {};
}

Config::Config_t Config::default_config() {
  return {
          {"cmake.cxx_compiler", "clang++"},
          {"cmake.c_compiler", "clang"},
          {"cmake.generator", "Ninja Multi-Config"},
  };
}
}// namespace haru

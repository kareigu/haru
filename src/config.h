#pragma once
#include "command.h"
#include "error.h"
#include <expected>
#include <filesystem>
#include <map>
#include <optional>

namespace haru {
constexpr const char* CONFIG_FILE_NAME = "haru.conf";
class Config {
public:
  using Config_t = std::map<std::string, std::string>;
  [[nodiscard("Handle error")]] static std::expected<void, Error> handle_config_command(const std::vector<std::string>& args, const Command::Flags_t flags);
  [[nodiscard("Handle error")]] static std::expected<std::string, Error> get_value(const std::string_view key, bool override_local = false);

private:
  [[nodiscard]] static const std::filesystem::path get_global_config_path();
  [[nodiscard]] static std::optional<const std::filesystem::path> get_local_config_path();
  [[nodiscard]] static std::expected<Config_t, Error> get_global_config();
  [[nodiscard]] static std::expected<Config_t, Error> get_local_config();
  [[nodiscard]] static std::expected<Config_t, Error> get_config(const std::filesystem::path& path);
  static std::expected<void, Error> write_value(const std::string& key, const std::string& value, bool global);

  [[nodiscard]] static Config_t default_config();
  Config();
};

}// namespace haru

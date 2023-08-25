#pragma once
#include "common.h"
#include <result.hpp>
#include <type_traits>

namespace haru {
class ArgsParser {
public:
  enum class ErrorType {
    Unknown,
  };
  using Error = Error<ErrorType>;

  ArgsParser() = delete;
  ArgsParser(const char* program_name, const char* version);
  ~ArgsParser() = default;

  cpp::result<int, Error> parse(int argc, char** argv);

private:
};
}// namespace haru

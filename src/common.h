#pragma once
#include <optional>
#include <string>
#include <type_traits>

#ifndef HARU_PRG_NAME
  #error Missing HARU_PRG_NAME
#endif

#ifndef HARU_VERSION
  #error Missing HARU_VERSION
#endif

namespace haru {
template<typename E>
concept Enum = std::is_enum_v<E>;

template<Enum E>
struct Error {
  E type;
  std::optional<std::string> message;
};
}// namespace haru

#include "error.h"

constexpr const char** init_type_as_string(auto max_value) {
  const char** array = new const char*[max_value];
  using IntType = haru::Error::TypeAsInt;

#define ERR_STR(VARIANT) array[static_cast<IntType>(haru::Error::VARIANT)] = #VARIANT
  ERR_STR(NO_INPUT);
  ERR_STR(INPUT_ERROR);
  ERR_STR(WRITE_ERROR);
  ERR_STR(ALREADY_EXISTS);
  ERR_STR(IO_ERROR);
  ERR_STR(GENERATE_ERROR);
  array[max_value] = "Unknown";

  return array;
}

namespace haru {
const char** Error::s_type_as_string = init_type_as_string(TYPE_MAX_VALUE);

const char* Error::type_as_string(Type type) {
  const auto* const S_TYPE_STRING = s_type_as_string[static_cast<TypeAsInt>(type)];
  if (type > UNKNOWN_ERROR || S_TYPE_STRING == nullptr)
    return "InvalidErrorType";
  return S_TYPE_STRING;
}
const char* Error::type_as_string() const {
  return type_as_string(m_type);
}
}// namespace haru

#include "error.h"

constexpr const char** init_type_as_string(auto max_value) {
  const char** array = new const char*[max_value];
  using IntType = haru::Error::TypeAsInt;

#define ERR_STR(VARIANT) array[static_cast<IntType>(haru::Error::VARIANT)] = #VARIANT
  ERR_STR(NoInput);
  ERR_STR(InputError);
  ERR_STR(Write);
  ERR_STR(AlreadyExists);
  ERR_STR(IOError);
  ERR_STR(Generate);
  array[max_value] = "Unknown";

  return array;
}

namespace haru {
const char** Error::s_type_as_string = init_type_as_string(TypeMaxValue);

const char* Error::type_as_string(Type type) {
  auto s_type_string = s_type_as_string[static_cast<TypeAsInt>(type)];
  if (type > Unknown || s_type_string == nullptr)
    return "InvalidErrorType";
  return s_type_string;
}
const char* Error::type_as_string() const {
  return type_as_string(m_type);
}
}// namespace haru

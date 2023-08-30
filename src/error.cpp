#include "error.h"

const char** init_type_as_string(auto max_value) {
  const char** array = new const char*[max_value];

  array[max_value] = "Unknown";

  return array;
}

namespace haru {
const char** Error::s_type_as_string = init_type_as_string(TypeMaxValue);

const char* Error::type_as_string(Type type) {
  if (type > Unknown)
    return "InvalidErrorType";
  return s_type_as_string[static_cast<TypeAsInt>(type)];
}
const char* Error::type_as_string() const {
  return type_as_string(m_type);
}
}// namespace haru

#include "error.h"

constexpr const char** init_type_as_string(auto max_value) {
  const char** array = new const char*[max_value];
  using IntType = haru::Error::TypeAsInt;

  array[static_cast<IntType>(haru::Error::NoInput)] = "NoInput";
  array[static_cast<IntType>(haru::Error::InputError)] = "InputError";
  array[static_cast<IntType>(haru::Error::Write)] = "WriteError";
  array[static_cast<IntType>(haru::Error::AlreadyExists)] = "AlreadyExists";
  array[static_cast<IntType>(haru::Error::IOError)] = "IOError";
  array[static_cast<IntType>(haru::Error::Generate)] = "Generate";
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

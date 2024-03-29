#pragma once

#define BAKE_IN(NAME)                    \
  extern unsigned char bake_in_##NAME[]; \
  extern unsigned int bake_in_##NAME##_len

BAKE_IN(clang_format);
BAKE_IN(gersemirc);
BAKE_IN(gitignore);
BAKE_IN(cmakelists);
BAKE_IN(cmake_dependencies);
BAKE_IN(cmake_compile_flags);
BAKE_IN(cmake_tidy);

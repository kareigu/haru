#pragma once

#ifndef HARU_PRG_NAME
  #error Missing HARU_PRG_NAME
#endif

#ifndef HARU_VERSION
  #error Missing HARU_VERSION
#endif

#if NDEBUG
  #define RELEASE_TYPE "Release"
#else
  #define RELEASE_TYPE "Debug"
#endif

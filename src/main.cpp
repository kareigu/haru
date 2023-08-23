#include <spdlog/spdlog.h>

#ifndef HARU_VERSION
  #error Missing HARU_VERSION
#endif

int main(int argc, char** argv) {
  spdlog::info("haru - v{:s}", HARU_VERSION);

  return 0;
}

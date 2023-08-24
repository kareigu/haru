cmake_minimum_required(VERSION 3.20)

include(FetchContent)

project(
  haru
  VERSION 0.1.0
  LANGUAGES CXX
)
add_compile_definitions(HARU_PRG_NAME="${PROJECT_NAME}")
add_compile_definitions(HARU_VERSION="${PROJECT_VERSION}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(NINJA_MULTICONFIG (CMAKE_GENERATOR STREQUAL "Ninja Multi-Config"))

set(FETCHCONTENT_QUIET FALSE)
set(FMT_INSTALL FALSE)
fetchcontent_declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.1.0
  GIT_PROGRESS TRUE
  USES_TERMINAL_DOWNLOAD TRUE
)
fetchcontent_makeavailable(fmt)

set(SPDLOG_FMT_EXTERNAL TRUE)
fetchcontent_declare(
  spdlog
  GIT_REPOSITORY https://github.com/kareigu/spdlog.git
  GIT_TAG easier_dual_sink
  GIT_PROGRESS TRUE
  USES_TERMINAL_DOWNLOAD TRUE
)
fetchcontent_makeavailable(spdlog)

fetchcontent_declare(
  Result
  GIT_REPOSITORY https://github.com/bitwizeshift/result.git
  GIT_TAG master
  GIT_PROGRESS TRUE
  USES_TERMINAL_DOWNLOAD TRUE
)
fetchcontent_makeavailable(Result)

set(ARGPARSE_INSTALL FALSE)
fetchcontent_declare(
  argparse
  GIT_REPOSITORY https://github.com/p-ranav/argparse.git
  GIT_TAG master
  GIT_PROGRESS TRUE
  USES_TERMINAL_DOWNLOAD TRUE
)
fetchcontent_makeavailable(argparse)

set(PROJECT_SOURCES src/main.cpp src/log_formatter.cpp)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
target_link_libraries(
  ${PROJECT_NAME}
  fmt
  spdlog
  Result
  argparse
)

if(NINJA_MULTICONFIG)
  set(OUTPUT_FILE_PATH "./Debug/${PROJECT_NAME}")
else()
  set(OUTPUT_FILE_PATH "./${PROJECT_NAME}")
endif()

add_custom_target(
  run
  COMMAND ${CMAKE_COMMAND} -E env ${OUTPUT_FILE_PATH}
  USES_TERMINAL VERBATIM
  DEPENDS ${PROJECT_NAME}
)

if(NINJA_MULTICONFIG)
  add_custom_target(
    release
    COMMAND ${CMAKE_COMMAND} -E env "ninja" "-f" "build-Release.ninja"
    USES_TERMINAL
  )
endif()

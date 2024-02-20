cmake_minimum_required(VERSION 3.20)

include(FetchContent)

project(
  haru
  VERSION 0.2.0
  LANGUAGES CXX
)
add_compile_definitions(HARU_PRG_NAME="${PROJECT_NAME}")
add_compile_definitions(HARU_VERSION="${PROJECT_VERSION}")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-diagnostics-color=always)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-fcolor-diagnostics)
endif()

if(WIN32)
    add_compile_options(-fansi-escape-codes)
endif()

set(NINJA_MULTICONFIG (CMAKE_GENERATOR STREQUAL "Ninja Multi-Config"))

set(FETCHCONTENT_QUIET FALSE)
set(FMT_INSTALL FALSE)
fetchcontent_declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.2.1
  GIT_PROGRESS TRUE
  USES_TERMINAL_DOWNLOAD TRUE
)
fetchcontent_makeavailable(fmt)

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
  args
  GIT_REPOSITORY https://github.com/Taywee/args.git
  GIT_TAG 6.4.6
  GIT_PROGRESS TRUE
  USES_TERMINAL_DOWNLOAD TRUE
)
fetchcontent_makeavailable(args)

set(PROJECT_SOURCES
    src/main.cpp
    src/project_info.cpp
    src/args_parser.cpp
    src/error.cpp
    src/command.cpp
    src/utils.cpp
    src/cmake_lists_generator.cpp
    src/file_operations.cpp
    src/log.cpp
)

#
# Bake configuration files into binary
#
find_program(XXD_CMD xxd)
if(XXD_CMD EQUAL XXD_CMD-NOTFOUND)
  message(FATAL_ERROR "xxd needs to be intalled and available in your path.")
endif()
set(BAKE_IN
    "${CMAKE_SOURCE_DIR}/generated/clang_format.cpp"
    "${CMAKE_SOURCE_DIR}/generated/cmake_format.cpp"
    "${CMAKE_SOURCE_DIR}/generated/gitignore.cpp"
)
add_custom_command(
  OUTPUT "${CMAKE_SOURCE_DIR}/generated/clang_format.cpp"
  COMMAND
    ${CMAKE_COMMAND} -E env "${XXD_CMD}" "-i" "-n" "bake_in_clang_format"
    "${CMAKE_SOURCE_DIR}/assets/clang-format" ">"
    "${CMAKE_SOURCE_DIR}/generated/clang_format.cpp"
  DEPENDS "${CMAKE_SOURCE_DIR}/assets/clang-format"
)
add_custom_command(
  OUTPUT "${CMAKE_SOURCE_DIR}/generated/cmake_format.cpp"
  COMMAND
    ${CMAKE_COMMAND} -E env "${XXD_CMD}" "-i" "-n" "bake_in_cmake_format"
    "${CMAKE_SOURCE_DIR}/assets/cmake-format.py" ">"
    "${CMAKE_SOURCE_DIR}/generated/cmake_format.cpp"
  DEPENDS "${CMAKE_SOURCE_DIR}/assets/cmake-format.py"
)
add_custom_command(
  OUTPUT "${CMAKE_SOURCE_DIR}/generated/gitignore.cpp"
  COMMAND
    ${CMAKE_COMMAND} -E env "${XXD_CMD}" "-i" "-n" "bake_in_gitignore"
    "${CMAKE_SOURCE_DIR}/assets/gitignore" ">"
    "${CMAKE_SOURCE_DIR}/generated/gitignore.cpp"
  DEPENDS "${CMAKE_SOURCE_DIR}/assets/gitignore"
)
#
# end: Bake configuration files into binary
#

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${BAKE_IN})
target_link_libraries(
  ${PROJECT_NAME}
  fmt
  Result
  args
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

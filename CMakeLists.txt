cmake_minimum_required(VERSION 3.27)

include(${CMAKE_SOURCE_DIR}/cmake/baking.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/compile_flags.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/dependencies.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/tidy.cmake)
include(FetchContent)

project(haru VERSION 0.3.1 LANGUAGES CXX)
set(NINJA_MULTICONFIG (CMAKE_GENERATOR STREQUAL "Ninja Multi-Config"))

fetch_project_dependencies()

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
    src/post_ops.cpp
)
add_bake_in_files()

add_executable(
    ${PROJECT_NAME}
    ${PROJECT_SOURCES}
    ${BAKE_IN}
)

target_compile_definitions(
    ${PROJECT_NAME}
    PRIVATE
        HARU_PRG_NAME="${PROJECT_NAME}"
)
target_compile_definitions(
    ${PROJECT_NAME}
    PRIVATE
        HARU_VERSION="${PROJECT_VERSION}"
)
set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD
            23
        CXX_STANDARD_REQUIRED
            TRUE
)

add_project_compile_flags(${PROJECT_NAME})
link_project_dependencies(${PROJECT_NAME})

add_clang_tidy(${PROJECT_NAME} ${CMAKE_CURRENT_BINARY_DIR})

if(NINJA_MULTICONFIG)
    foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOLOWER ${CONFIG} BUILD_NAME)
        add_custom_target(
            "run-${BUILD_NAME}"
            COMMAND
                ${CMAKE_COMMAND} -E env
                "${CMAKE_CURRENT_BINARY_DIR}/${CONFIG}/${PROJECT_NAME}"
            USES_TERMINAL
            VERBATIM
            DEPENDS
                ${PROJECT_NAME}:$<CONFIG>
        )
    endforeach()
endif()
add_custom_target(
    run
    COMMAND
        ${CMAKE_COMMAND} -E env "$<TARGET_FILE:${PROJECT_NAME}>"
    USES_TERMINAL
    VERBATIM
    DEPENDS
        ${PROJECT_NAME}:$<CONFIG>
)

if(NINJA_MULTICONFIG)
    foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOLOWER ${CONFIG} CONFIG)
        add_custom_target(
            ${CONFIG}
            COMMAND
                ${CMAKE_COMMAND} --build "${CMAKE_CURRENT_BINARY_DIR}"
                "--config" "${CONFIG}"
            USES_TERMINAL
        )
    endforeach()
endif()

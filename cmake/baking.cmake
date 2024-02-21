#
# Bake configuration files into binary
#
function(add_bake_in_files)
    find_program(
        XXD
        xxd
        REQUIRED
        NO_CACHE
        DOC
            "xxd needed to generate .cpp source files for baking default configuration files into the binary"
    )
    set(BAKE_IN
        "${CMAKE_SOURCE_DIR}/generated/clang_format.cpp"
        "${CMAKE_SOURCE_DIR}/generated/gersemirc.cpp"
        "${CMAKE_SOURCE_DIR}/generated/gitignore.cpp"
    )
    add_custom_command(
        OUTPUT
            "${CMAKE_SOURCE_DIR}/generated/clang_format.cpp"
        COMMAND
            ${CMAKE_COMMAND} -E env "${XXD}" "-i" "-n" "bake_in_clang_format"
            "${CMAKE_SOURCE_DIR}/assets/clang-format" ">"
            "${CMAKE_SOURCE_DIR}/generated/clang_format.cpp"
        DEPENDS
            "${CMAKE_SOURCE_DIR}/assets/clang-format"
    )
    add_custom_command(
        OUTPUT
            "${CMAKE_SOURCE_DIR}/generated/gersemirc.cpp"
        COMMAND
            ${CMAKE_COMMAND} -E env "${XXD}" "-i" "-n" "bake_in_gersemirc"
            "${CMAKE_SOURCE_DIR}/assets/gersemirc" ">"
            "${CMAKE_SOURCE_DIR}/generated/gersemirc.cpp"
        DEPENDS
            "${CMAKE_SOURCE_DIR}/assets/gersemirc"
    )
    add_custom_command(
        OUTPUT
            "${CMAKE_SOURCE_DIR}/generated/gitignore.cpp"
        COMMAND
            ${CMAKE_COMMAND} -E env "${XXD}" "-i" "-n" "bake_in_gitignore"
            "${CMAKE_SOURCE_DIR}/assets/gitignore" ">"
            "${CMAKE_SOURCE_DIR}/generated/gitignore.cpp"
        DEPENDS
            "${CMAKE_SOURCE_DIR}/assets/gitignore"
    )
    set(BAKE_IN ${BAKE_IN} PARENT_SCOPE)
endfunction()
#
# end: Bake configuration files into binary
#


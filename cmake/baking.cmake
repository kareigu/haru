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
    set(filenames
        "clang_format"
        "gersemirc"
        "gitignore"
    )
    set(output_paths)

    foreach(file ${filenames})
        add_custom_command(
            OUTPUT
                "${CMAKE_SOURCE_DIR}/generated/${file}.cpp"
            COMMAND
                ${CMAKE_COMMAND} -E env "${XXD}" "-i" "-n" "bake_in_${file}"
                "${CMAKE_SOURCE_DIR}/assets/${file}" ">"
                "${CMAKE_SOURCE_DIR}/generated/${file}.cpp"
            DEPENDS
                "${CMAKE_SOURCE_DIR}/assets/${file}"
        )
        list(APPEND output_paths "${CMAKE_SOURCE_DIR}/generated/${file}.cpp")
        message("Added build step for ${file}")
    endforeach()
    set(BAKE_IN ${output_paths} PARENT_SCOPE)
endfunction()
#
# end: Bake configuration files into binary
#

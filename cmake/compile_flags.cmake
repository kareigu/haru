function(add_project_compile_flags)
    set(CLANG_WARNINGS
        -Wall
        -Wextra
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
        -Wimplicit-fallthrough
        -Wno-gnu-statement-expression
    )

    set(GCC_WARNINGS
        ${CLANG_WARNINGS}
        -Wmisleading-indentation
        -Wduplicated-cond
        -Wduplicated-branches
        -Wlogical-op
        -Wuseless-cast
    )

    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options(-diagnostics-color=always)
        add_compile_options(${GCC_WARNINGS})
    elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES ".*Clang")
        add_compile_options(-fcolor-diagnostics)
        add_compile_options(${CLANG_WARNINGS})
    elseif(MSVC)
        message(FATAL_ERROR "Unsupported")
    endif()

    if(WIN32)
        add_compile_options(-fansi-escape-codes)
    endif()
endfunction()


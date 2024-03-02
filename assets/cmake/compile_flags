function(add_project_compile_flags project_name)
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
        set(WARNINGS_CXX
            -diagnostics-color=always
            ${GCC_WARNINGS}
        )
    elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES ".*Clang")
        set(WARNINGS_CXX
            -fcolor-diagnostics
            ${CLANG_WARNINGS}
        )
    elseif(MSVC)
        message(FATAL_ERROR "Unsupported")
    endif()

    if(WIN32)
        set(WARNINGS_CXX
            ${WARNINGS_CXX}
            -fansi-escape-codes
        )
    endif()
    set(WARNINGS_C ${WARNINGS_CXX})
    target_compile_options(
        ${project_name}
        PRIVATE
            $<$<COMPILE_LANGUAGE:CXX>:${WARNINGS_CXX}>
            $<$<COMPILE_LANGUAGE:C>:${WARNINGS_C}>
    )
endfunction()

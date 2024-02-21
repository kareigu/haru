function(fetch_project_dependencies)
    set(FETCHCONTENT_QUIET FALSE)
    set(FMT_INSTALL FALSE)
    fetchcontent_declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 10.2.1
        GIT_PROGRESS TRUE
        USES_TERMINAL_DOWNLOAD
        TRUE
        SYSTEM
    )
    fetchcontent_makeavailable(fmt)

    fetchcontent_declare(
        Result
        GIT_REPOSITORY https://github.com/bitwizeshift/result.git
        GIT_TAG master
        GIT_PROGRESS TRUE
        USES_TERMINAL_DOWNLOAD
        TRUE
        SYSTEM
    )
    fetchcontent_makeavailable(Result)

    set(ARGPARSE_INSTALL FALSE)
    set(ARGPARSE_BUILD_TESTS FALSE)
    fetchcontent_declare(
        argparse
        GIT_REPOSITORY https://github.com/p-ranav/argparse.git
        GIT_TAG v3.0
        GIT_PROGRESS TRUE
        USES_TERMINAL_DOWNLOAD
        TRUE
        SYSTEM
    )
    fetchcontent_makeavailable(argparse)
endfunction()

function(link_project_dependencies PROJECT_NAME)
    target_link_libraries(
        ${PROJECT_NAME}
        fmt
        Result
        argparse
    )
endfunction()


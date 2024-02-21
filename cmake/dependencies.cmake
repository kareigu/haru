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
    set(ARGS_BUILD_EXAMPLE FALSE)
    set(ARGS_BUILD_UNITTESTS FALSE)
    fetchcontent_declare(
        args
        GIT_REPOSITORY https://github.com/Taywee/args.git
        GIT_TAG 6.4.6
        GIT_PROGRESS TRUE
        USES_TERMINAL_DOWNLOAD
        TRUE
        SYSTEM
    )
    fetchcontent_makeavailable(args)
endfunction()

function(link_project_dependencies PROJECT_NAME)
    target_link_libraries(
        ${PROJECT_NAME}
        fmt
        Result
        args
    )
endfunction()


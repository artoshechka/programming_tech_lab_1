# Creates an EXCLUDE_FROM_ALL gtest binary and registers it in ctest and the aggregate target.
function(create_test target_name)
    if(NOT FILE_OBSERVER_BUILD_TESTS)
        return()
    endif()

    if(ARGC LESS 2)
        message(FATAL_ERROR "create_test(<target_name> <sources...> [LINK_LIBS <libs...>]) requires at least one source file")
    endif()

    if(NOT TARGET GTest::gtest_main)
        find_package(GTest REQUIRED)
        include(GoogleTest)
    endif()

    set(options)
    set(one_value_args)
    set(multi_value_args LINK_LIBS)
    cmake_parse_arguments(CREATE_TEST "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    add_executable(${target_name} EXCLUDE_FROM_ALL ${CREATE_TEST_UNPARSED_ARGUMENTS})
    target_link_libraries(${target_name}
        PRIVATE
            GTest::gtest_main
            ${CREATE_TEST_LINK_LIBS}
    )

    gtest_discover_tests(${target_name})

    set_property(GLOBAL APPEND PROPERTY FILE_OBSERVER_TEST_TARGETS ${target_name})
endfunction()

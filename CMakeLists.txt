cmake_minimum_required(VERSION 3.8)

project(cpponsea2019)
set(CMAKE_CXX_STANDARD 17)

find_package(GTest)
if(NOT ${GTEST_FOUND}) # Download gtest if not installed
    message("No system gtest use external project")

    # Download and unpack googletest at configure time
    configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
    execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download" )
    execute_process(COMMAND "${CMAKE_COMMAND}" --build .
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download" )

    # Prevent GoogleTest from overriding our compiler/linker options
    # when building with Visual Studio
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # Add googletest directly to our build. This adds
    # the following targets: gtest, gtest_main, gmock
    # and gmock_main
    add_subdirectory("${CMAKE_BINARY_DIR}/googletest-src" "${CMAKE_BINARY_DIR}/googletest-build")
endif()

include_directories(lib)
enable_testing()

add_subdirectory(demo_approvals_and_catch2)
add_subdirectory(demo_approvals_and_googletest)

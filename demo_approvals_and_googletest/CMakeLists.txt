cmake_minimum_required(VERSION 3.8)
project(demo_approvals_and_googletest)

set(CMAKE_CXX_STANDARD 17)
set(SOURCE_FILES
    main.cpp
    Test01.cpp
    Test02.cpp
    Test03CustomReporters.cpp
    Test04ConsoleReporter.cpp
    Test05VerifyAll.cpp
    Test06VerifyAllCombinations.cpp
    )
add_executable(${PROJECT_NAME} ${SOURCE_FILES} )
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} gtest gtest_main)

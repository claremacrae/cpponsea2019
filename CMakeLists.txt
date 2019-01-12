cmake_minimum_required(VERSION 3.8)

project(cpponsea2019)
set(CMAKE_CXX_STANDARD 17)

include_directories(lib)
enable_testing()

add_subdirectory(demo_approvals_and_catch2)

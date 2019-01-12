cmake_minimum_required(VERSION 3.8)
project(gilded_rose_refactoring_kata)

set(CMAKE_CXX_STANDARD 17)
set(SOURCE_FILES
    GildedRose.cc
    GildedRose.h
    GildedRoseTest.cpp
)
add_executable(${PROJECT_NAME} ${SOURCE_FILES} )
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} gtest gtest_main)

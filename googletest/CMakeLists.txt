cmake_minimum_required(VERSION 3.10)
project(MyTestProject)

add_subdirectory(googletest/googletestcode)
include_directories(googletest/googletestcode/include)

add_executable(my_test_executable mainTest.cpp)
target_link_libraries(my_test_executable gtest)
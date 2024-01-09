cmake_minimum_required(VERSION 3.10)
project(MyTestProject)


add_subdirectory(googletestcode)
include_directories(googletestcode/include)

add_executable(my_test_executable mainTest.cpp)
target_link_libraries(my_test_executable gtest)
enable_testing()
add_test(NAME my_test COMMAND my_test_executable)
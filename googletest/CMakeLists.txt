cmake_minimum_required(VERSION 3.10) # cmakeのバージョンを指定
project(MyTestProject) # プロジェクト名を指定


add_subdirectory(googletestcode) # テストコードのディレクトリを指定
include_directories(googletestcode/include) # テストコードのインクルードディレクトリを指定

add_executable(my_test_executable mainTest.cpp) # テストコードの実行ファイルを指定
target_link_libraries(my_test_executable gtest) # テストコードの実行ファイルにgtestをリンク
enable_testing()
add_test(NAME my_test COMMAND my_test_executable ) # テストコードの実行ファイルをテストとして登録
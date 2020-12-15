cmake_minimum_required(VERSION 3.17)
project(LR1Parser)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib/googletest-master)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(LR1Parser SHARED src/Grammar.cpp src/LR1Parser.cpp)

add_executable(ParserExecutable main.cpp)
target_link_libraries(ParserExecutable LR1Parser)

add_executable(CTest test/build_table_test.cpp)
target_link_libraries(CTest gtest gtest_main LR1Parser)


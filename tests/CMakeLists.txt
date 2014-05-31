cmake_minimum_required (VERSION 2.6)
project (solitaire-tests)

include_directories("${PROJECT_SOURCE_DIR}/../")
list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

add_executable(test_init test_init.cpp)
add_executable(test_speed test_speed.cpp)
add_executable(test_speed_prepare test_speed_prepare.cpp)
add_executable(test_correctness test_correctness.cpp)
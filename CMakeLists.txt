# Project to learn basic ray traing in C++

cmake_minimum_required(VERSION 3.19.0)

# set compiler before project declaration to 
# avoid an infinite loop in config
set(CMAKE_CXX_COMPILER /usr/bin/clang++)

# set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -Werror")

project(raytracing_weekend)

# basic ppm executable
add_executable(ppm_test
               ${CMAKE_CURRENT_SOURCE_DIR}/src/ppm_test.cpp)

# Don't want gnu C++ extentions
set_target_properties(ppm_test
                      PROPERTIES CXX_EXTENSIONS OFF)

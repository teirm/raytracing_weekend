# Project to learn basic ray traing in C++

cmake_minimum_required(VERSION 3.19.0)

# set compiler before project declaration to 
# avoid an infinite loop in config
set(CMAKE_CXX_COMPILER /usr/bin/clang++)

# set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-g -O3 -Wall -Wextra -Werror")

project(raytracing_weekend)

# basic ppm executable
add_executable(inOneWeekend
               ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

# Don't want gnu C++ extentions
set_target_properties(inOneWeekend
                      PROPERTIES CXX_EXTENSIONS OFF)

# Add includes
target_include_directories(inOneWeekend
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# std::thread depends on pthread on linux and freebsd
# Don't care about windows anytime soon
target_link_libraries(inOneWeekend PRIVATE pthread)

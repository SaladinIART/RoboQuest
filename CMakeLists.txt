cmake_minimum_required(VERSION 3.10)
project(RoboQuest VERSION 0.1)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add executable
add_executable(RoboQuest src/main.cpp)

# Include directories
target_include_directories(RoboQuest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
cmake_minimum_required(VERSION 3.0)
project(VkDisplayTest)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lava)
add_subdirectory(lava-extras)

add_executable(VkDisplayTest "main.cpp")
target_link_libraries(VkDisplayTest PUBLIC lava lava-extras-display)

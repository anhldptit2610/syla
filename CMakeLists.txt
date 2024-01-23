cmake_minimum_required(VERSION 3.28.1)
project(syla)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(core)
add_subdirectory(desktop)
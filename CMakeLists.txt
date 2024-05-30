cmake_minimum_required(VERSION 3.22.1)

project(Raycasting)

find_package(SDL2 REQUIRED)
include_directories(Raycasting ${SDL2_INCLUDE_DIRS})

add_executable(Raycasting main.cpp)
target_link_libraries(Raycasting ${SDL2_LIBRARIES})
cmake_minimum_required(VERSION 3.15)

project(TaskCLI CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCE "src/*.cpp")

add_executable(task-cli ${SOURCE})
target_include_directories(task-cli PUBLIC src include)

cmake_minimum_required(VERSION 3.15)

project(TaskCLI CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(task-cli
    src/cli.cpp
    src/main.cpp
    src/json.cpp
)

target_compile_options(task-cli PRIVATE
  $<$<CONFIG:Debug>:-Wall>
  $<$<CONFIG:Debug>:-Wextra>
  $<$<CONFIG:Debug>:-O0>
  $<$<CONFIG:Debug>:-g>
  $<$<CONFIG:Release>:-O3>
  $<$<CONFIG:Release>:-DNDEBUG>
  $<$<CONFIG:Release>:-march=native>
)

target_include_directories(task-cli PRIVATE include)

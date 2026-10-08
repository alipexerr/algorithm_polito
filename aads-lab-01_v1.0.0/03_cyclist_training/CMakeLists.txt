cmake_minimum_required(VERSION 3.20)

project(cyclist_training C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

file(GLOB_RECURSE STUDENT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

add_executable(cyclist_training ${STUDENT_SOURCES})

target_include_directories(cyclist_training PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

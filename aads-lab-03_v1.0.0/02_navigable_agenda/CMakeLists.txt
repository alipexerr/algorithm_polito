cmake_minimum_required(VERSION 3.20)

project(navigable_agenda C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

file(GLOB_RECURSE STUDENT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

add_executable(navigable_agenda ${STUDENT_SOURCES})

target_include_directories(navigable_agenda PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

cmake_minimum_required(VERSION 3.20)

project(task_planner C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

file(GLOB_RECURSE MODULE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)
list(REMOVE_ITEM MODULE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c")

add_library(task_planner_modules ${MODULE_SOURCES})
target_include_directories(task_planner_modules PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_executable(task_planner src/main.c)
target_link_libraries(task_planner PRIVATE task_planner_modules)

enable_testing()

add_library(unity STATIC tests/unity/unity.c)
target_include_directories(unity PUBLIC tests/unity)

add_executable(task_planner_tests
    tests/test_task_planner.c
    tests/support/allocation_tracker.c
)
target_include_directories(task_planner_tests PRIVATE src tests)
target_link_libraries(task_planner_tests PRIVATE
    unity
    task_planner_modules
)
target_link_options(task_planner_tests PRIVATE
    -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc
    -Wl,--wrap=free -Wl,--wrap=strdup
)

add_test(
    NAME task_planner_tests
    COMMAND task_planner_tests
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

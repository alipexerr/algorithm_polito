cmake_minimum_required(VERSION 3.20)

project(auditorium_seating C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

file(GLOB_RECURSE MODULE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)
list(REMOVE_ITEM MODULE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c")

add_library(auditorium_seating_modules ${MODULE_SOURCES})
target_include_directories(auditorium_seating_modules PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_executable(auditorium_seating src/main.c)
target_link_libraries(auditorium_seating PRIVATE auditorium_seating_modules)

enable_testing()

add_library(unity STATIC tests/unity/unity.c)
target_include_directories(unity PUBLIC tests/unity)

if(APPLE)
    set(ALLOCATION_TRACKER_SOURCE
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/support/allocation_tracker_macos.c"
    )
    target_compile_options(auditorium_seating_modules PRIVATE
        -include
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/support/allocation_tracker_macos.h"
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(ALLOCATION_TRACKER_SOURCE
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/support/allocation_tracker.c"
    )
else()
    message(FATAL_ERROR
        "Unsupported operating system for allocation tracking: ${CMAKE_SYSTEM_NAME}"
    )
endif()

add_executable(auditorium_seating_tests
    tests/test_seating_plan.c
    ${ALLOCATION_TRACKER_SOURCE}
)
target_include_directories(auditorium_seating_tests PRIVATE src tests)
target_link_libraries(auditorium_seating_tests PRIVATE
    unity
    auditorium_seating_modules
)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_options(auditorium_seating_tests PRIVATE
        -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc
        -Wl,--wrap=free -Wl,--wrap=strdup
    )
endif()

add_test(
    NAME auditorium_seating_tests
    COMMAND auditorium_seating_tests
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

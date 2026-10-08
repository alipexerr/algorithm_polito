cmake_minimum_required(VERSION 3.20)
project(bike_sharing_report C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

file(GLOB_RECURSE MODULE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)
list(REMOVE_ITEM MODULE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c")

add_library(station_report ${MODULE_SOURCES})
target_include_directories(station_report PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_executable(bike_sharing_report src/main.c)
target_link_libraries(bike_sharing_report PRIVATE station_report)

enable_testing()

add_library(unity STATIC
    tests/unity/unity.c
)

target_include_directories(unity PUBLIC
    tests/unity
)

add_executable(bike_sharing_tests
    tests/test_station_report.c
)

target_include_directories(bike_sharing_tests PRIVATE
    src
)

target_link_libraries(bike_sharing_tests PRIVATE unity)
target_link_libraries(bike_sharing_tests PRIVATE station_report)

add_test(
    NAME bike_sharing_tests
    COMMAND bike_sharing_tests
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

cmake_minimum_required(VERSION 3.20)

project(word_index C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

file(GLOB_RECURSE WORD_INDEX_ARRAY_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/array_version/*.c"
)
list(REMOVE_ITEM WORD_INDEX_ARRAY_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/array_version/main.c"
)

add_library(word_index_array_module ${WORD_INDEX_ARRAY_SOURCES})
target_include_directories(word_index_array_module PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/array_version"
)

add_executable(word_index_array
    src/array_version/main.c
)
target_link_libraries(word_index_array PRIVATE word_index_array_module)

file(GLOB_RECURSE WORD_INDEX_POINTER_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/pointer_version/*.c"
)
list(REMOVE_ITEM WORD_INDEX_POINTER_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/pointer_version/main.c"
)

add_library(word_index_pointer_module ${WORD_INDEX_POINTER_SOURCES})
target_include_directories(word_index_pointer_module PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/pointer_version"
)

add_executable(word_index_pointer
    src/pointer_version/main.c
)
target_link_libraries(word_index_pointer PRIVATE word_index_pointer_module)

enable_testing()

add_library(unity STATIC tests/unity/unity.c)
target_include_directories(unity PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/unity"
)

add_executable(word_index_array_tests
    tests/array_version/test_word_index.c
    tests/support/allocation_tracker.c
)
target_include_directories(word_index_array_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/array_version"
    "${CMAKE_CURRENT_SOURCE_DIR}/tests"
)
target_link_libraries(word_index_array_tests PRIVATE
    unity
    word_index_array_module
)
target_link_options(word_index_array_tests PRIVATE
    -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc
    -Wl,--wrap=free -Wl,--wrap=strdup
)

add_executable(word_index_pointer_tests
    tests/pointer_version/test_word_index.c
    tests/support/allocation_tracker.c
)
target_include_directories(word_index_pointer_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/pointer_version"
    "${CMAKE_CURRENT_SOURCE_DIR}/tests"
)
target_link_libraries(word_index_pointer_tests PRIVATE
    unity
    word_index_pointer_module
)
target_link_options(word_index_pointer_tests PRIVATE
    -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc
    -Wl,--wrap=free -Wl,--wrap=strdup
)

add_test(
    NAME word_index_array_tests
    COMMAND word_index_array_tests
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_test(
    NAME word_index_pointer_tests
    COMMAND word_index_pointer_tests
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

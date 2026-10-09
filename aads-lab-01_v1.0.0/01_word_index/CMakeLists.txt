cmake_minimum_required(VERSION 3.20)

project(word_index C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# ============================================================
# Array version
# ============================================================

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

# ============================================================
# Pointer version
# ============================================================

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

# ============================================================
# Executable programs
# ============================================================

add_executable(word_index
src/array_version/main.c
)
target_link_libraries(word_index PRIVATE word_index_array_module)

add_executable(word_index_pointer
src/pointer_version/main.c
)
target_link_libraries(word_index_pointer PRIVATE word_index_pointer_module)

# ============================================================
# Test infrastructure
# ============================================================

enable_testing()

add_library(unity STATIC tests/unity/unity.c)

target_include_directories(unity PUBLIC
"${CMAKE_CURRENT_SOURCE_DIR}/tests/unity"
)

# Select allocation tracker by operating system.
if(APPLE)
set(ALLOCATION_TRACKER_SOURCE
"${CMAKE_CURRENT_SOURCE_DIR}/tests/support/allocation_tracker_macos.c"
)

# Intercept allocations made by the modules by rewriting
# allocator calls to the macOS tracking functions.
foreach(target word_index_array_module word_index_pointer_module)
target_compile_options(${target} PRIVATE
-include
"${CMAKE_CURRENT_SOURCE_DIR}/tests/support/allocation_tracker_macos.h"
)
endforeach()

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
set(ALLOCATION_TRACKER_SOURCE
"${CMAKE_CURRENT_SOURCE_DIR}/tests/support/allocation_tracker.c"
)
else()
message(FATAL_ERROR
"Unsupported operating system for allocation tracking: ${CMAKE_SYSTEM_NAME}"
)
endif()

# ============================================================
# Array tests
# ============================================================

add_executable(word_index_array_tests
tests/array_version/test_word_index.c
${ALLOCATION_TRACKER_SOURCE}
)

target_include_directories(word_index_array_tests PRIVATE
"${CMAKE_CURRENT_SOURCE_DIR}/src/array_version"
"${CMAKE_CURRENT_SOURCE_DIR}/tests"
"${CMAKE_CURRENT_SOURCE_DIR}/tests/support"
)

target_link_libraries(word_index_array_tests PRIVATE
unity
word_index_array_module
)

# ============================================================
# Pointer tests
# ============================================================

add_executable(word_index_pointer_tests
tests/pointer_version/test_word_index.c
${ALLOCATION_TRACKER_SOURCE}
)

target_include_directories(word_index_pointer_tests PRIVATE
"${CMAKE_CURRENT_SOURCE_DIR}/src/pointer_version"
"${CMAKE_CURRENT_SOURCE_DIR}/tests"
"${CMAKE_CURRENT_SOURCE_DIR}/tests/support"
)

target_link_libraries(word_index_pointer_tests PRIVATE
unity
word_index_pointer_module
)

# GNU linker wrapping is available on Linux, not Apple's linker.
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
foreach(target word_index_array_tests word_index_pointer_tests)
target_link_options(${target} PRIVATE
-Wl,--wrap=malloc
-Wl,--wrap=calloc
-Wl,--wrap=realloc
-Wl,--wrap=free
-Wl,--wrap=strdup
)
endforeach()
endif()

# ============================================================
# Register tests
# ============================================================

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
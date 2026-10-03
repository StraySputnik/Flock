cmake_minimum_required(VERSION 3.31)
project(flock)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/$<CONFIG>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/$<CONFIG>)

include(cmake/CPM.cmake)

if (UNIX)
    add_compile_options(
            -Wall
            -Werror
            -fsanitize=address
    )

    add_link_options(
            -fsanitize=address
    )
endif ()

add_subdirectory(core)
add_subdirectory(sandbox)
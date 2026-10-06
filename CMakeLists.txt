cmake_minimum_required(VERSION 3.31)
project(flock)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/$<CONFIG>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/$<CONFIG>)

include(cmake/CPM.cmake)

set(SANITIZER OFF)
if (UNIX)
    add_compile_options(
            -Wall
            -Werror
    )

    if (SANITIZER)
        add_compile_options(-fsanitize=address)
        add_link_options(-fsanitize=address)
    endif ()
endif ()

add_subdirectory(core)
add_subdirectory(sandbox)
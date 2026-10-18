cmake_minimum_required(VERSION 3.31)
project(flock)

include(cmake/CPM.cmake)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/$<CONFIG>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/$<CONFIG>)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

add_subdirectory(third_party/stbi)
add_subdirectory(core)
add_subdirectory(sandbox)
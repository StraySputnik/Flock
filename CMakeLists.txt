cmake_minimum_required(VERSION 3.31)
project(flock)

set(CPM_SOURCE_CACHE "$ENV{HOME}/.cpm")
include(cmake/CPM.cmake)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/$<CONFIG>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/$<CONFIG>)
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/third_party)

add_subdirectory(core)
add_subdirectory(sandbox)
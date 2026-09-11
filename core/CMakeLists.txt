project(flock_core)

add_library(
        ${PROJECT_NAME} SHARED
        src/common.hpp
        include/memory/allocator.hpp
        include/maybe.hpp
        include/result.hpp
        src/memory/allocator.cpp
        include/collect/vector.hpp
        include/collect/string.hpp
        src/collect/string.cpp
        include/collect/map.hpp
        include/collect/list.hpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(${PROJECT_NAME} PRIVATE FLK_SHARED_BUILD)

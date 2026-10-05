project(flock_core)

add_library(
        ${PROJECT_NAME} SHARED
        src/common.hpp
        include/memory/allocator.hpp
        src/memory/allocator.cpp
        include/maybe.hpp
        include/collect/vector.hpp
        include/collect/string.hpp
        src/collect/string.cpp
        include/hash.hpp
        include/collect/map.hpp
        include/ecs/group.hpp
        include/ecs/registry.hpp
        src/ecs/registry.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(${PROJECT_NAME} PRIVATE FLK_SHARED_BUILD)
add_compile_definitions(FLK_SHARED_BUILD)

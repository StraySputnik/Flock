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
        include/ecs/schedule.hpp
        src/ecs/schedule.cpp
        include/ecs/world.hpp
        src/ecs/world.cpp
        include/app.hpp
        src/app.cpp
        include/platform/window.hpp
        include/platform/sdl/sdl_window.hpp
        src/platform/sdl/sdl_window.cpp
        include/event/event.hpp
        include/memory/ref.hpp
        src/event/event.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(${PROJECT_NAME} PUBLIC FLK_SHARED_BUILD FLK_PLATFORM_SDL3)

CPMAddPackage("gh:libsdl-org/SDL#release-3.4.18")

target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3)
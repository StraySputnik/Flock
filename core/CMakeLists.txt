project(flock_core CXX)

add_library(
        ${PROJECT_NAME} SHARED
        src/common.hpp
        include/memory/allocator.hpp
        src/memory/allocator.cpp
        include/maybe.hpp
        include/collect/vec.hpp
        include/collect/string.hpp
        src/collect/string.cpp
        include/hash.hpp
        include/collect/hash_map.hpp
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
        include/math/vector.hpp
        include/math/matrix.hpp
        include/math/math.hpp
        include/math/quaternion.hpp
        include/graphics/renderer.hpp
        include/math/color.hpp
        include/asset/asset.hpp
        src/asset/asset.cpp
        include/file_system/path.hpp
        src/file_system/path.cpp
        include/collect/pair.hpp
        include/file_system/file.hpp
        src/file_system/file.cpp
        include/file_system/directory.hpp
        src/file_system/directory.cpp
        include/debug/print.hpp
        include/debug/log.hpp
        src/debug/log.cpp
        include/time/time.hpp
        src/time/time.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(${PROJECT_NAME} PUBLIC FLK_SHARED_BUILD FLK_PLATFORM_SDL3)

set(SANITIZER OFF)
if (UNIX)
    target_compile_options(
            ${PROJECT_NAME} PRIVATE
            -Wall
            -Werror
    )

    if (SANITIZER)
        target_compile_options(${PROJECT_NAME} PUBLIC -fsanitize=address)
        target_link_options(${PROJECT_NAME} PUBLIC -fsanitize=address)
    endif ()
endif ()

CPMAddPackage("gh:libsdl-org/SDL#release-3.4.18")
CPMAddPackage("gh:tinyobjloader/tinyobjloader#release")
target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3 stbi tinyobjloader_c)
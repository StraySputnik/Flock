project(sokol C)

add_library(
        ${PROJECT_NAME} STATIC
        include/sokol_gfx.h
        include/sokol_time.h
        include/sokol_audio.h
        src/sokol.c
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC wgpu::native)
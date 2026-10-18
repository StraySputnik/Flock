project(stbi C)

add_library(
        ${PROJECT_NAME} STATIC
        include/stbi.h
        src/stbi.c
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
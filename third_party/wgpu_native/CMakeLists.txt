project(wgpu_native)

set(WGPU_VERSION "v29.0.1.1")

if(WIN32)
    set(WGPU_PLATFORM "windows")
    set(WGPU_COMPILER "-msvc")
elseif(APPLE)
    set(WGPU_PLATFORM "macos")
    set(WGPU_COMPILER "")
else()
    set(WGPU_PLATFORM "linux")
    set(WGPU_COMPILER "")
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|ARM64|aarch64)$")
    set(WGPU_ARCH "aarch64")
else()
    set(WGPU_ARCH "x86_64")
endif()

set(WGPU_ASSET "wgpu-${WGPU_PLATFORM}-${WGPU_ARCH}${WGPU_COMPILER}-release")

CPMAddPackage(
        NAME wgpu_native
        URL "https://github.com/gfx-rs/wgpu-native/releases/download/${WGPU_VERSION}/${WGPU_ASSET}.zip"
        DOWNLOAD_ONLY YES
)

add_library(wgpu_native STATIC IMPORTED GLOBAL)
add_library(wgpu::native ALIAS wgpu_native)

if(WIN32)
    set(WGPU_LIB_FILE "${wgpu_native_SOURCE_DIR}/lib/wgpu_native.lib")
else()
    set(WGPU_LIB_FILE "${wgpu_native_SOURCE_DIR}/lib/libwgpu_native.a")
endif()

set_target_properties(wgpu_native PROPERTIES
        IMPORTED_LOCATION "${WGPU_LIB_FILE}"
        INTERFACE_INCLUDE_DIRECTORIES "${wgpu_native_SOURCE_DIR}/include"
)

if(WIN32)
    target_link_libraries(wgpu_native INTERFACE
            d3dcompiler ws2_32 userenv bcrypt ntdll opengl32 propsys runtimeobject)
elseif(APPLE)
    target_link_libraries(wgpu_native INTERFACE
            "-framework Metal" "-framework QuartzCore" "-framework CoreFoundation")
else()
    target_link_libraries(wgpu_native INTERFACE m dl pthread)
endif()
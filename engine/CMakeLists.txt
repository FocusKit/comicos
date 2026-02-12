qt_add_library(comicos_engine STATIC
    src/BrushDab.cpp
    src/BrushEngine.cpp
    src/TileCache.cpp
    src/Compositor.cpp
)

target_include_directories(comicos_engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(comicos_engine PUBLIC
    comicos_core
    Qt6::Core
    Qt6::Gui
)

# Extension point: SIMD / platform-specific optimizations
# if(COMICOS_PLATFORM STREQUAL "windows")
#     target_compile_options(comicos_engine PRIVATE /arch:AVX2)
# elseif(NOT IOS)
#     target_compile_options(comicos_engine PRIVATE -mavx2 -mfma)
# endif()

qt_add_library(comicos_render STATIC
    include/render/CanvasItem.h
    include/render/CanvasRenderer.h
    include/render/RenderBackend.h
    include/render/TileRenderer.h
    src/CanvasRenderer.cpp
    src/RenderBackend.cpp
    src/CanvasItem.cpp
    src/TileRenderer.cpp
)

target_include_directories(comicos_render PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(comicos_render PUBLIC
    comicos_core
    comicos_engine
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
)

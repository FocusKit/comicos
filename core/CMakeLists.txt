qt_add_library(comicos_core STATIC
    src/Types.cpp
    src/Tile.cpp
    src/TileManager.cpp
    src/Layer.cpp
    src/LayerStack.cpp
    src/Stroke.cpp
    src/History.cpp
    src/Document.cpp
)

target_include_directories(comicos_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(comicos_core PUBLIC
    Qt6::Core
    Qt6::Gui
)

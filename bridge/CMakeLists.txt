qt_add_library(comicos_bridge STATIC
    include/bridge/AppController.h
    include/bridge/DocumentModel.h
    src/AppController.cpp
    src/DocumentModel.cpp
)

target_include_directories(comicos_bridge PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(comicos_bridge PUBLIC
    comicos_core
    comicos_engine
    comicos_render
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
)

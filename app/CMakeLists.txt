# --- Executable ---
qt_add_executable(comicos
    main.cpp
)

# --- QML Module ---
qt_add_qml_module(comicos
    URI Comicos
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/Toolbar.qml
        qml/CanvasView.qml
        qml/LayerPanel.qml
        qml/StatusBar.qml
        qml/controls/IconButton.qml
        qml/controls/SegmentedControl.qml
        qml/controls/Modal.qml
)

# Theme.qml is a singleton — must be marked explicitly for Qt 6.5
set_source_files_properties(qml/Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)
qt_target_qml_sources(comicos QML_FILES qml/Theme.qml)

target_link_libraries(comicos PRIVATE
    comicos_core
    comicos_engine
    comicos_render
    comicos_bridge
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Svg
)

# --- Platform-specific settings ---

# Windows
if(WIN32)
    # set_target_properties(comicos PROPERTIES
    #     WIN32_EXECUTABLE TRUE
    # )
    # Extension point: Windows manifest, icon resource
    # target_sources(comicos PRIVATE resources/comicos.rc)
endif()

# macOS
if(APPLE AND NOT IOS)
    set_target_properties(comicos PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.comicos.app"
        MACOSX_BUNDLE_BUNDLE_NAME "Comicos"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
    )
endif()

# iOS
if(IOS)
    set_target_properties(comicos PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.comicos.app"
        XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "2"  # iPad only
        XCODE_ATTRIBUTE_SUPPORTS_MACCATALYST "NO"
    )
    # Extension point: iOS-specific settings
    # - Apple Pencil support
    # - Split View / Slide Over
    # - Files app integration
endif()

# Install
install(TARGETS comicos
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

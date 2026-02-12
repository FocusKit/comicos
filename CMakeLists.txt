cmake_minimum_required(VERSION 3.21)
project(Comicos VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# --- Platform Detection ---
if(APPLE AND NOT IOS)
    set(COMICOS_PLATFORM "macos")
elseif(IOS)
    set(COMICOS_PLATFORM "ios")
elseif(WIN32)
    set(COMICOS_PLATFORM "windows")
else()
    set(COMICOS_PLATFORM "linux")
endif()

message(STATUS "Comicos: building for platform = ${COMICOS_PLATFORM}")

# --- Qt 6 ---
find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickControls2
    QuickDialogs2
    ShaderTools
    Svg
)

qt_standard_project_setup(REQUIRES 6.5)

# --- Global compile options ---
if(MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_compile_definitions(
    COMICOS_VERSION="${PROJECT_VERSION}"
    COMICOS_PLATFORM_${COMICOS_PLATFORM}
)

# --- Sub-modules ---
add_subdirectory(core)
add_subdirectory(engine)
add_subdirectory(render)
add_subdirectory(bridge)
add_subdirectory(shaders)
add_subdirectory(app)

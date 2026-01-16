cmake_minimum_required(VERSION 3.20)

project(Picture
    VERSION 0.1.0
    DESCRIPTION "Feature-rich photo editor"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/ImageCanvas.cpp
    src/ResizeDialog.cpp
    src/CropOverlay.cpp
    src/RotateDialog.cpp
    src/ImageProcessor.cpp
    src/AdjustmentsPanel.cpp
)

set(HEADERS
    src/MainWindow.h
    src/ImageCanvas.h
    src/ResizeDialog.h
    src/CropOverlay.h
    src/RotateDialog.h
    src/ImageProcessor.h
    src/AdjustmentsPanel.h
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

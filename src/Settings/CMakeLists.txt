cmake_minimum_required(VERSION 3.30)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find required Qt packages
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Define the Settings library
add_library(Settings STATIC
    settingsmanager.h
    settingsmanager.cpp
    settingsdialog.h
    settingsdialog.cpp
    settingsdialog.ui
)

# Link Qt libraries
target_link_libraries(Settings PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

# Include directories
target_include_directories(Settings PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

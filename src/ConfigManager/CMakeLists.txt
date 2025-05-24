cmake_minimum_required(VERSION 3.30)

project(ConfigManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Gui Widgets Network REQUIRED)

# Create static library
add_library(ConfigManager STATIC
    configmanager.h
    configmanager.cpp
    ButtonTipManager.h
    ButtonTipManager.cpp
)

target_link_libraries(ConfigManager 
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
)

target_include_directories(ConfigManager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Set output directories
set_target_properties(ConfigManager PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
) 
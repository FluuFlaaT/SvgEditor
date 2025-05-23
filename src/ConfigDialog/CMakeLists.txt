cmake_minimum_required(VERSION 3.30)

project(ConfigDialog LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

# Create static library instead of shared
add_library(ConfigDialog STATIC
    configdialog.h
    configdialog.cpp
)

target_link_libraries(ConfigDialog 
    Qt5::Core
    Qt5::Widgets
    ConfigManager
)

target_include_directories(ConfigDialog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Set output directories
set_target_properties(ConfigDialog PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
) 
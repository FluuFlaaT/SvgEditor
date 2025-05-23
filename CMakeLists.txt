cmake_minimum_required(VERSION 3.30)
project(SvgEditor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include(C:/code/kingsoft/thirdparty_install/vcpkg/scripts/buildsystems/vcpkg.cmake)

find_package(Qt5 COMPONENTS Core Gui Widgets Svg LinguistTools Xml REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(src/Commands)
add_subdirectory(src/ConfigManager)
add_subdirectory(src/ConfigDialog)
add_subdirectory(src/SvgEditor)
add_subdirectory(src/CoreSvgEngine)
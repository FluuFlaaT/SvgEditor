cmake_minimum_required(VERSION 3.30)
project(SvgEditor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON) 
set(CMAKE_AUTORCC ON) 
set(CMAKE_AUTOUIC ON)

include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_subdirectory(src/SvgEditor_Core Core) 
add_subdirectory(src/SvgEditor_UI UI)     
add_subdirectory(src/SvgEditor_App App)   

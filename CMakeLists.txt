cmake_minimum_required(VERSION 3.30)
project(SvgEditor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON) 
set(CMAKE_AUTORCC ON) 
set(CMAKE_AUTOUIC ON)

include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

find_package(Qt5 COMPONENTS Core Gui Widgets Svg REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(src/CommandManager CommandManager)
add_subdirectory(src/ConfigManager ConfigManager)
add_subdirectory(src/CoreSvgEngine CoreSvgEngine)
add_subdirectory(src/DrawingTools DrawingTools)
add_subdirectory(src/FileIOManager FileIOManager)
add_subdirectory(src/LoggingService LoggingService)
add_subdirectory(src/QtUiLayer QtUiLayer)

# 启用测试
enable_testing()
add_subdirectory(tests)

# 解决 MSVC C4819 报 warning 的问题
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

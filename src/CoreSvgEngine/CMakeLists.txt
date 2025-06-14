﻿set(TARGET_NAME CoreSvgEngine)

file(GLOB_RECURSE SOURCES *.cpp)
file(GLOB_RECURSE HEADERS *.h)

add_library(${TARGET_NAME} SHARED
    ${SOURCES}
    ${HEADERS}
)

find_package(tinyxml2 CONFIG REQUIRED)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(${TARGET_NAME} PRIVATE
    CORESVGENGINE_LIBRARY
)

target_link_libraries(${TARGET_NAME} PRIVATE
    tinyxml2::tinyxml2
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Svg
    Qt5::Xml
)

set_target_properties(${TARGET_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)

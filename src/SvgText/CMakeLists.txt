set(TARGET_NAME SvgText)

set(SOURCES
    SvgText.cpp
)

set(HEADERS
    SvgText.h
)

add_library(${TARGET_NAME} STATIC
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

find_package(tinyxml2 CONFIG REQUIRED)

target_link_libraries(${TARGET_NAME} PUBLIC
    Qt5::Core
)

target_link_libraries(${TARGET_NAME} PRIVATE
    tinyxml2::tinyxml2
)

set_target_properties(${TARGET_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

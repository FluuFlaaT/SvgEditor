set(TARGET_NAME Commands)

set(SOURCES
    Command.cpp
    CommandManager.cpp
    AddShapeCommand.cpp
    RemoveShapeCommand.cpp
)

set(HEADERS
    Command.h
    CommandManager.h
    CommandUtils.h
    AddShapeCommand.h
    RemoveShapeCommand.h
    SvgEditorForwards.h
)

add_library(${TARGET_NAME} STATIC
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(${TARGET_NAME} PUBLIC
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Svg
)

set_target_properties(${TARGET_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

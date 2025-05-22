set(TARGET_NAME SvgEditor)

set(SOURCES
    main.cpp
    MainWindow.cpp
    LeftSideBar.cpp
    RightAttrBar.cpp
)

set(HEADERS
    MainWindow.h
    LeftSideBar.h
    RightAttrBar.h
)

file(GLOB UI_FILES *.ui)

add_executable(${TARGET_NAME}
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
)

set_target_properties(${TARGET_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(${TARGET_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Svg
)
set(TARGET_NAME QtUiLayer)

set(SOURCES
    main.cpp
    MainWindow.cpp
    SvgDisplayWidget.cpp
    DrawingLayer.cpp
    LeftSideBar.cpp
    RightAttrBar.cpp
)

set(HEADERS
    MainWindow.h
    SvgDisplayWidget.h
    DrawingLayer.h
    LeftSideBar.h
    RightAttrBar.h
)

file(GLOB UI_FILES *.ui)

add_executable(${TARGET_NAME}
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
)

target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/LoggingService
)

target_link_libraries(${TARGET_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Svg
)

target_link_libraries(${TARGET_NAME} PRIVATE
    CoreSvgEngine
    LoggingService
)

set_target_properties(${TARGET_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
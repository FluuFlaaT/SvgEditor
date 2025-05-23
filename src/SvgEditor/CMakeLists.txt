set(TARGET_NAME SvgEditor)

qt5_add_resources(qrc_FILES
                edtSource.qrc
)

set(SOURCES
    main.cpp
    MainWindow.cpp
    LeftSideBar.cpp
    RightAttrBar.cpp
    CanvasArea.cpp
    ShapeToolBar.cpp
    EditableTextItem.cpp
)

set(HEADERS
    mainwindow.h
    leftsidebar.h
    rightattrbar.h
    canvasarea.h
    shapetoolbar.h
    editabletextitem.h
)

file(GLOB UI_FILES *.ui)

add_executable(${TARGET_NAME}
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
    ${qrc_FILES}
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
    Qt5::Xml
    CoreSvgEngine
    Commands
    ConfigManager
    ConfigDialog
)


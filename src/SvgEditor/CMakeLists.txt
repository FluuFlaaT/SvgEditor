set(TARGET_NAME SvgEditor)

# Translation files
set(TS_FILES
    translations/svgeditor_zh_CN.ts
    translations/svgeditor_en.ts
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

# Generate translation files (.ts -> .qm)
qt5_create_translation(QM_FILES ${SOURCES} ${HEADERS} ${TS_FILES})

qt5_add_resources(qrc_FILES
                edtSource.qrc
                translations.qrc
)

file(GLOB UI_FILES *.ui)

add_executable(${TARGET_NAME} WIN32
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
    ${qrc_FILES}
    ${QM_FILES}  # Add QM_FILES to ensure they are built
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

# Ensure ConfigDialog DLL is copied to output directory
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:ConfigDialog>
    $<TARGET_FILE_DIR:${TARGET_NAME}>
)


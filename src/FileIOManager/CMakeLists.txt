set(CMAKE_AUTOMOC ON)

add_library(FileIOManager
    FileIOManager.cpp
    FileIOManager.h
)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

target_link_libraries(FileIOManager PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    CoreSvgEngine
    LoggingService
)

target_include_directories(FileIOManager PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src)
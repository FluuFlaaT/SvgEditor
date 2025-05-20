set(CMAKE_AUTOMOC ON)

add_library(FileIOManager
    FileIOManager.cpp
    FileIOManager.h
)

# 包含Qt组件
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

# 链接依赖库
target_link_libraries(FileIOManager PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    CoreSvgEngine
    LoggingService
)

# 包含目录
target_include_directories(FileIOManager PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src)
# add_library 用于创建库
# STATIC 表示静态库, SHARED 表示动态库 (DLL on Windows)
# 为了简单起见，我们先用 STATIC，如果需要 DLL，可以改为 SHARED
add_library(SvgEditor_UI STATIC
        mainwindow.cpp
        mainwindow.ui
)

# 指定头文件目录，以便其他模块可以找到它们
target_include_directories(SvgEditor_UI PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接 Qt Widgets (因为它是一个UI库)
target_link_libraries(SvgEditor_UI PUBLIC Qt5::Widgets)
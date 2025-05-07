add_executable(SvgEditor_App WIN32
        main.cpp
)

# 链接 SvgEditor_UI 库和 Qt Widgets
target_link_libraries(SvgEditor_App PRIVATE
        SvgEditor_UI
        Qt5::Widgets
)
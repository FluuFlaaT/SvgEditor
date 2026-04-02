# Test executable that runs all unit tests
add_executable(SvgEditorTests
    CoreSvgEngine/TestSvgDocument.cpp
    CoreSvgEngine/TestSvgElement.cpp
    CoreSvgEngine/TestIntegration.cpp
    Commands/TestCommandManager.cpp
    Commands/TestAddShapeCommand.cpp
    Commands/TestRemoveShapeCommand.cpp
    Commands/TestNewCommands.cpp
    Core/TestShapeType.cpp
)

target_link_libraries(SvgEditorTests
    PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        GTest::gtest_main
        CoreSvgEngine
        Commands
)

include(GoogleTest)
gtest_discover_tests(SvgEditorTests)
